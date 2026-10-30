# Decisions

This file records project-level decisions so the curriculum and firmware do not drift.

## D-001 — ESP32-first, IDE-later

**Decision:** Use the ESP32 as an interactive teaching device before using it as a programming-course target.

**Reason:** Concepts can be learned through physical cause and effect before syntax becomes a distraction.

## D-002 — Core Seven first

**Decision:** Stabilize seven foundational activities before expanding the catalog.

**Reason:** A small coherent progression is more valuable than many disconnected demos.

## D-003 — Three activity modes

**Decision:** Mature activities use PLAY, LEARN, and CHALLENGE modes.

**Reason:** The same activity can support fun, guided discovery, and practice without forcing instruction into every session.

## D-004 — Predict before reveal

**Decision:** Use PREDICT → ACT → OBSERVE → EXPLAIN when it improves the lesson.

**Reason:** Prediction turns device interaction into experimentation and makes misconceptions visible.

## D-005 — Hardware abstraction

**Decision:** Curriculum/activity logic should not directly own board-specific display, touch, LED, sensor, or radio implementation details.

**Reason:** The educational work should survive hardware changes and be easier to test.

## D-006 — No fabricated hardware configuration

**Decision:** Do not commit guessed pins, board models, controllers, or library versions as canonical.

**Reason:** Hardware values must come from vendor documentation and physical-device acceptance, not speculation.

## D-007 — Offline-first Phase 1

**Decision:** Phase 1 does not require accounts, personal-data collection, cloud services, ads, or network access.

**Reason:** None are needed to teach the foundational concepts and they add complexity unrelated to the learning goal.

## D-008 — Baseline before refactor

**Decision:** MIKEY-002 imports and verifies the known-good jackpot behavior before restructuring it.

**Reason:** A reproducible behavioral baseline prevents architecture work from obscuring hardware or migration defects.

## D-009 — Preserve accepted physical baseline

**Decision:** The physically accepted MIKEY-002 Jackpot sketch remains immutable as a standalone reference while MIKEY-003 migrates its behavior into the shared application shell.

**Accepted baseline SHA:**

```text
fc1735a0e6c34bec0caf89e5f03eac8317549f5a
```

**Physical acceptance date:** 2026-09-18.

**Reason:** Keeping the accepted physical implementation available makes regressions during abstraction and navigation work easy to isolate.


## D-010 — Test learning rules without hardware when practical

**Decision:** Keep deterministic activity rules and session state in small hardware-independent models when they can be separated cleanly from rendering, timing, and physical I/O. Run those models in native CI tests in addition to compiling the Arduino firmware.

**Reason:** Fast deterministic tests make educational behavior easier to verify, preserve the hardware-abstraction boundary, and reduce the amount of logic that can only be checked on the physical CrowPanel. Physical acceptance is still required for display geometry, touch/encoder behavior, LEDs, timing feel, and integrated regressions.

## D-011 — Optional adult-approved HTTPS OTA without home gateway

**Decision (2026-10-22, planning only):** Use ESP32-initiated outbound HTTPS firmware download for future remote Wi-Fi updates, not Tailscale subnet routing, an always-on home Linux host, or an ESP32 internet-facing upload server. Require approved signed images, authenticated release metadata, proper certificate validation, idle-only installation, exact board/partition compatibility, and physical USB rescue qualification. Preserve offline-first learning and do not touch the immutable MIKEY-002 baseline.

**Reason:** Remote maintenance can be delivered using existing Wi-Fi and an approved read-only firmware origin without extra hardware; authorization, image authenticity and recovery must be designed independently of transport. Existing `huge_app` partition configuration has **No OTA**, so a verified USB-installed dual-slot partition table is an explicit prerequisite. See [MIKEY-011 Plan](MIKEY-011-HTTPS-OTA-PLAN.md).

**Release gate:** Plan/documentation changes do not authorize firmware flashes, repartition, signing-key creation, secret provisioning, hosting publication, or live OTA deployment.
