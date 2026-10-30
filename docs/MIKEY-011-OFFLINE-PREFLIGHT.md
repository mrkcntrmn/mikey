# MIKEY-011 — Offline OTA preparation and release procedure

**Status (2026-10-30): source-only candidate in draft PR #15. NO release, Wi-Fi client or hardware acceptance.**

## Decisions already locked

- Target device: Elecrow CrowPanel ESP32-S3, documented 16 MB flash / 8 MB OPI PSRAM.
- Preserve all seven PLAY modes and the immutable MIKEY-002 standalone Jackpot sketch.
- Adopt a **static Cloudflare R2 HTTPS distribution origin** (public read-only firmware assets), with GitHub for source and CI. No Cloudflare Worker, device login, inbound endpoint, VPN or dedicated home computer is needed for the first release.
- Firmware authenticity is verified on the ESP32 using the pinned RSA-2048 public key, *not* by trusting the download origin. Signed release metadata must also be verified before selecting firmware. Public downloads alone are not authorization.
- Owner's public key **DER/SPKI SHA-256 fingerprint** (safe to commit):
  `f7e95de7bc1e316ea8e3121815738eaf522d8cff0c50f6d877103d065e8e2265`
- Owner's private key is on an offline LUKS2 USB vault; backup is GPG-encrypted. No actual public PEM was provided to the repository. **The fingerprint cannot recreate a public PEM**. Future device verifier integration needs the PUBLIC PEM only.
- Do **not** put the private key, LUKS passphrase, GPG passphrase, Wi-Fi password, R2 credentials, or test keys in Git.
- No automatic rollback guarantees yet. Actual Arduino core/bootloader behavior and physical partition readback must be verified before any real update.

## Preflight source artifacts

- `firmware/mikey-tech-lab/MikeyTechLab/partitions.csv`: **provisional**, dual 6 MiB OTA slots; 16 MB total. Arduino picks a sketch-local `partitions.csv` for builds. Adding this table to a build changes partition allocation but does not change a device unless flashed.
- `tools/ota/preflight.py`: checks partition inventory, offsets, alignment, overlaps, flash bounds and +512-byte signature trailer with 20% growth allowance.
- `tools/ota/sign_release.py`: **offline-only** RSA-2048/SHA-256 RSA-PSS release packager, signed metadata and verifier. Matches the 512-byte padded trailer produced by Arduino-ESP32 **3.3.8** `bin_signing.py`. Requires Python `cryptography` package. No network calls.
- `tests/test_ota_tools.py`: ephemeral key tests, never the owner's key.
- `.github/workflows/ota-preflight.yml`: exact 16 MB / OPI compile, offline test, unsigned CI artifact; no publish/signing secrets or firmware uploads to devices.

Reference: https://raw.githubusercontent.com/espressif/arduino-esp32/3.3.8/tools/bin_signing.py and https://raw.githubusercontent.com/espressif/arduino-esp32/3.3.8/libraries/Update/src/Update.h

## Preparing a release later (not needed now)

1. On the trusted Kali machine, mount your LUKS2 vault only when intentionally signing an **approved, reviewed** release. Do not enter passphrases into GitHub or a cloud service.
2. Install dependencies in a local Python virtual environment:
   `python3 -m venv /tmp/mikey-ota-venv && /tmp/mikey-ota-venv/bin/pip install 'cryptography>=42,<48'`.
3. Get the **unsigned** application `.bin` from the exact SHA qualified in CI, or compile that same SHA locally with the exact FQBN. Check source provenance; never sign an unreviewed artifact.
4. After choosing a real HTTPS delivery domain (substitute it for `https://ota.example.invalid`), sign OFFLINE. Example:
   ```bash
   python3 tools/ota/sign_release.py sign \
     --firmware /path/to/MikeyTechLab.ino.bin \
     --private-key /mnt/mikey-vault/private/ota-signing-rsa2048.pem \
     --public-key /mnt/mikey-vault/public/ota-signing-rsa2048.pub.pem \
     --origin https://ota.example.invalid \
     --version 0.1.1 --channel canary \
     --build-sha FULL40CHARACTERCOMMITID \
     --output-dir /path/to/existing/empty/release-folder
   ```
5. Verify the result *before considering publication*:
   ```bash
   python3 tools/ota/sign_release.py verify \
     --image /path/to/existing/empty/release-folder/MikeyTechLab-0.1.1-signed.bin \
     --manifest /path/to/existing/empty/release-folder/manifest-0.1.1.json \
     --public-key /mnt/mikey-vault/public/ota-signing-rsa2048.pub.pem
   ```
6. Lock and unmount the USB vault immediately afterward. Keep the private signing key offline. This script never uploads to Cloudflare.

**Current hold:** Do not run signing or publish commands yet; no OTA client has been integrated into the ESP32 application and no physical recovery has been qualified.

## Planned Cloudflare R2 publishing (NOT configured)

- Dedicated small R2 bucket, immutable paths: `releases/{channel}/{version}/MikeyTechLab-{version}-signed.bin` and `releases/{channel}/{version}/manifest-{version}.json`. Align the uploaded object paths with the URL inside the signed manifest.
- Use an owner-controlled HTTPS custom domain for device downloads. Do not use development-only `r2.dev` as the production origin.
- Use a short-cache `{channel}/latest.json` discovery pointer if desired. This pointer is **untrusted input**; the device must verify signed manifest contents and reject older versions, mismatched board/layout/channel and unsigned images. Never flash based on the pointer alone.
- Never overwrite a published versioned object. Enable storage protection/retention appropriate for the release process. Restrict write credentials to the owner's machine or separately authorized publishing environment.
- HTTPS certificate *chain and hostname* must validate on the ESP32; do not use `setInsecure()`.
- Any separate signature/manifest signing key rotation requires an explicit cross-signed or USB re-provisioning strategy. One key for the initial pilot is intentional.
- No Cloudflare resources, DNS records, buckets, tokens or release objects are created by this PR.

## Hardware-only acceptance holds

Before an **explicitly authorized** first USB flash:
- Physically read ESP32 flash size, PSRAM and installed partition table. Verify the prototype `partitions.csv` matches the real 16 MB module.
- Compile exact source and collect app size/PSRAM/headroom readings; verify the bootloader's **rollback build setting**, USB recovery and NVS migration/erase effects. No automatic rollback claim until proven.
- Export the public PEM on a trusted machine, fingerprint-check it and embed **only the public key** in the firmware verifier. Disable all update checks until the full signature and signed-manifest client passes source tests.
- Implement adult-only local Wi-Fi provisioning with no secrets in code or logs. Offline PLAY must remain available.
- Test invalid signed manifest, bad image signature, downgrade/replay, mixed hardware, wrong layout, expired TLS cert, disconnection, interrupted download and post-boot health.
- Perform a separately authorized USB conversion followed by a single signed OTA canary, full seven-game regression and USB rescue rehearsal.

**Success criterion for this PR:** a green *source-only* preflight and reviewable plan, not real OTA readiness.
