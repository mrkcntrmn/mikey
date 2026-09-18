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
