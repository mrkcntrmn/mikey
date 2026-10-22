# MIKEY-011 — Remote Wi-Fi Firmware Updates (HTTPS OTA)

**Status: PLAN ONLY — 2026-10-22.** No OTA-enabled image has been built, flashed, published, or physically accepted. This workstream does not change the accepted Core Seven PLAY firmware, the immutable MIKEY-002 Jackpot baseline, or MIKEY-010's learning-review priority.

## Decision and scope

Use **ESP32-initiated outbound HTTPS OTA** rather than Tailscale, a spare Linux computer, an inbound upload endpoint, or internet-exposed ESP32 ports. A signed firmware release is approved by the adult maintainer and fetched over the device's existing home Wi-Fi. The games stay **offline-first**: Wi-Fi failure must never block starting, playing, exiting, or navigating an activity. No account, child identifier, cloud telemetry, advertising, or always-on home gateway is required.

The ESP32 checks a read-only HTTPS update manifest on a conservative schedule (or on an adult-initiated check), while idle. It must not install in the middle of a game. An adult approval of a release, together with device-side signature verification and release policy, is the authorization mechanism. Physical confirmation can be an optional mode, but must **not** be a prerequisite for the remote-update use case. Progress and result are shown on the 240×240 display; long-running network work must not block normal input/game timing.

### Intended path

```text
Maintainer approves tagged release
  → GitHub Actions compiles and tests *the exact source*
  → approved release signing (private key never in repository)
  → versioned binary + signed metadata are published to read-only HTTPS origin
  → CrowPanel makes outbound TLS-verified request
  → verifies applicability, version and signature
  → downloads to inactive OTA partition
  → verifies image, schedules safe reboot, validates boot
  → reports outcome on device (no remote personal telemetry)
```

GitHub Actions is a *build/release system*, not the ESP32's OTA server. The first delivery origin may be immutable versioned GitHub Release assets **if a public download is acceptable**, or a low-cost object-store/CDN HTTPS origin. GitHub Actions ephemeral artifacts are **not** a permanent device download API. Do not select hosting or begin publication until the actual redirect, certificate, URL-stability, and access model is tested on the board. Public delivery alone is not authorization: it relies on device-enforced signing. No bearer token or private signing key may be shipped inside public firmware.

## Repository-specific discovery / blocking issue

- Target: Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display, ESP32-S3 family, documented **16 MB flash / 8 MB OPI PSRAM**; see [Hardware](HARDWARE.md). Physical flash readout must confirm this on the actual unit.
- Accepted physical upload FQBN: `esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi`. This is explicitly a **No OTA** partition scheme. It cannot be reused for OTA firmware installation.
- Current CI `.github/workflows/firmware-compile.yml` compiles both sketches with the generic `esp32:esp32:esp32s3` FQBN, which does **not** prove the accepted flash/PSRAM/partition configuration.
- The current `MikeyTechLab.ino` has **no Wi-Fi or OTA implementation**. It uses a shared hardware adapter, activities, screen state and a nonblocking `loop()` with `delay(1)`. OTA should be an independently tested system service rather than logic inside individual games.
- The accepted MIKEY-002 Jackpot baseline sketch is **immutable**. It must not receive OTA modifications.
- Existing CI pins ESP32 Arduino core **3.3.8**, LovyanGFX **1.2.26**, NeoPixel **1.15.1**. Confirm OTA/signing API behavior against this pinned toolchain, not just upstream `master`.
- **Unknown until measured:** actual compiled app size, actual partition table installed on the device, OTA slot sizing, rollback build options/bootloader behavior, free heap while rendering and downloading, Wi-Fi reliability, and ability to recover after changing partition layout.

**Hard prerequisite:** design a **16 MB dual-OTA-slot** partition layout with `otadata`, `ota_0` and `ota_1`, sized for the **signed** app plus growth margin. Preserve the data partitions required by the application; do not guess at a final CSV. Changing partition layout requires an **initial local USB flash** with an explicit recovery procedure. An OTA-only binary cannot change the partition table/bootloader safely.

## Version, security, and privacy contract

1. **Application binary signing is mandatory.** The device embeds only a public verification key. Prefer the supported Arduino-ESP32 `Update` signing facility if it is validated under **core 3.3.8**; otherwise make a separate reviewed design decision rather than silently falling back to checksum-only checks. SHA-256 is an additional integrity check, **not a substitute for an authenticated signature**.
2. Manifest should be **authenticated/signed** and contain a bounded schema: `product`, `hardware_id`, `channel`, `version`, `build_sha`, `image_url`, `image_size`, `sha256`, `signing_key_id`, `min_supported_version`, `partition_layout_id`. Use immutable URLs/digests; reject missing/unknown/mismatched fields, unknown keys, cross-board binaries and incompatible layouts. Validate manifest authentication before using its metadata, and firmware signature before activation.
3. Enforce HTTPS certificate-chain **and hostname validation** with a maintained trust anchor; never use `setInsecure()`. Redirects must remain allowlisted and independently TLS-valid. Use timeouts, bounded download sizes, and fail closed.
4. Release signing credentials belong in restricted **maintainer-controlled release secrets**, separated from untrusted pull-request workflows. Builds on PRs/test branches **cannot publish or sign**. Require deliberate adult approval of the exact commit, build binary hash and channel. Define key rotation/revocation before field rollout.
5. Reject unauthorized downgrades/replays by verified release policy plus monotonically increasing version state; test how nonvolatile state and a USB recovery reset affect this. Device-side eFuse anti-rollback and secure boot are **not enabled or promised in this plan**; these are separate irreversible hardware-security decisions requiring review.
6. No Wi-Fi passwords, server credentials, signing keys, child names, account IDs, or diagnostic serial logs with secrets in Git. Wi-Fi can be configured locally by an adult; store it locally with explicit erase/reprovision behavior. No cloud presence tracking required.
7. Never allow arbitrary remote code upload, unsigned developer firmware, an unauthenticated `/update` HTTP server, or public inbound access. Tailscale is **not** part of this first release.

## Device behavior

- **Normal boot:** games appear promptly; network connection attempts are background/time-bounded. Offline play and all seven existing activities work indefinitely with Wi-Fi absent.
- **Check:** only an approved channel; idle periods by default. Cache last-seen valid manifest version; avoid repeated polling and unnecessary battery/network use. Do not install during any PLAY/LEARN/CHALLENGE session. Adult manual check may be added outside the existing Core Seven home navigation if screen space permits.
- **Download:** verify app/partition compatibility, inactive-slot capacity and available memory; show an `UPDATE` progress page; keep visual feedback and watchdog servicing responsive; permit sensible cancellation before image activation.
- **Activation:** only after hash/signature verification and full write success; show the target version and reboot reason. No update if identical version or incompatible policy. On aborted/failed download, keep current application usable.
- **Post-boot health:** verify basic hardware/display/input initialization and reach Home before marking healthy if the bootloader has validated rollback support. **Do not claim automatic rollback without proving it is enabled with the selected Arduino build and bootloader.** If rollback is unavailable, stop rollout and retain explicit physical USB rescue as the failure mode, or qualify rollback in a dedicated build first.
- **Status:** locally display current version, update availability/phase, clear error category and last local result. No remote telemetry required.

## Staged work orders and acceptance gates

### MIKEY-011A — Board and partition preflight (source/USB planning only)

- Record physical flash/PSRAM and current partition table; inspect compile size in bytes for existing Core Seven source.
- Produce and source-control a reviewed 16 MB partition CSV with two adequate app slots and required data partitions; reserve realistic future growth.
- Qualify exact physical FQBN versus CI; retain separate immutable Jackpot baseline compile and a known-good USB rescue binary/procedure.
- Determine whether existing Wi-Fi/NVS state is present and what re-flash will erase. Document provisioning/reprovisioning.
- **PASS:** exact toolchain compiles new partition-configured app, partition offsets do not overlap, signed firmware fits both slots with margin, USB rescue is documented. **No flash yet.**

### MIKEY-011B — Offline-safe OTA service implementation

- Add system-level network/OTA service, version/manifest schema, TLS trust, signature verification and memory-bounded download; all disabled behind explicit compile/runtime gate until accepted.
- No business logic changes to Core Seven; no new GPIO assignments. Run host-side unit tests for policy/parser/failure paths and firmware compile with exact board settings.
- **PASS:** invalid signature, downgrade, wrong hardware, oversized payload, bad TLS, corrupt transfer, timeout and low-memory cases all fail closed; offline-first operation proven in tests.

### MIKEY-011C — Release pipeline and controlled hosting (dark)

- Add a separately approved build-sign-publish workflow: versioned immutable assets, manifest/binary hashes, commit/tree/toolchain provenance, signed metadata, release approval, key custody and channel control.
- Dry run without releasing to any device. Prefer an initially nonpublic staging channel. Do not expose credentials via pull_request or logs.
- **PASS:** only approved sources sign/publish; rejected or unapproved runs cannot issue a distributable update; client validates the real host's TLS and redirects.

### MIKEY-011D — Single-device physical acceptance

- Owner expressly authorizes **one local USB installation** of the OTA-capable partition table and firmware after backup/recovery readiness. No unattended rollout.
- Confirm Wi-Fi setup, boot/Home time, touch/encoder, LEDs, and all seven PLAY games with Wi-Fi offline and online.
- Publish a uniquely versioned **non-game-changing canary**; verify full HTTPS transfer, correct version screen, safe idle update, integrity enforcement, successful boot and known-good recovery.
- Test interrupted download/power loss (within a controlled environment), invalid/bad signature, stale manifest, incompatible firmware, no Wi-Fi, low power if applicable, and confirmed rollback (if present). Demonstrate working USB recovery path.
- **PASS:** signed canary installed once without changing gameplay, all seven regressions pass, offline operation survives, recovery is physically demonstrated; capture the actual firmware SHA and device receipt.

### MIKEY-011E — Controlled operating procedure

- Lock down release promotion, audit/retention, manual revocation, key rotation, failure handling and recovery documentation.
- Define a deliberate adult-approved update policy (manual release by default; device checks only when idle). No automatic unsigned releases.
- **PASS:** one repeatable, independently reviewable release procedure; no unnecessary home hardware, inbound port, account or child data.

## Non-goals and holds

- No immediate USB flash, repartition, Wi-Fi credential provisioning, cloud deployment, new key generation, GitHub release, or OTA publication is authorized by this planning document.
- No change to MIKEY-010 educational priorities or seven accepted PLAY behaviors.
- No automatic firmware rollback promise until the real bootloader/Arduino build demonstrates it.
- No hardware eFuse burn/secure boot activation as a side effect.
- No child-facing mandatory connectivity, update prompt, account or telemetry.

## Reference material

- [Espressif ESP32-S3 OTA architecture and rollback](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/ota.html)
- [Arduino-ESP32 Update library and signing](https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/README.md) — verify compatibility specifically against pinned **3.3.8**
- [Hardware and accepted upload configuration](HARDWARE.md)
- [Core Seven accepted behavior and evidence limits](CORE-SEVEN-CLOSEOUT-2026-10-15.md)
- [MIKEY-010 learning review](MIKEY-010-READINESS.md)
