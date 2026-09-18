# Roadmap

## MIKEY-001 — Foundation

**Status:** complete.

Delivered:

- project vision and concept-before-syntax learning philosophy;
- Core Seven curriculum;
- PLAY / LEARN / CHALLENGE modes;
- PREDICT → ACT → OBSERVE → EXPLAIN learning loop;
- reusable activity specification;
- hardware-abstraction direction.

## MIKEY-002 — Hardware Capture + Jackpot Baseline

**Status:** complete.

Physical acceptance completed **2026-09-18**.

Accepted source baseline:

```text
fc1735a0e6c34bec0caf89e5f03eac8317549f5a
```

Delivered:

1. Identified the target Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display from vendor documentation and prototype dependency fingerprints.
2. Captured the vendor pin map and Arduino upload configuration.
3. Pinned a current Arduino toolchain/dependency set.
4. Added a standalone Jackpot baseline that avoids LVGL and an external CST816D dependency.
5. Added automated compile validation.
6. Uploaded and physically accepted the baseline on the CrowPanel.

All physical exit criteria passed:

- firmware compiles in CI;
- firmware uploads to the physical device;
- clockwise screen direction verified;
- touch start/stop verified;
- encoder speed direction verified;
- speed 25–1000% verified;
- five WS2812 LEDs verified;
- jackpot fixed at the top verified;
- sustained high-speed operation shows no reset/freeze.

The accepted standalone sketch remains immutable as the hardware oracle while later milestones migrate its behavior into the shared app shell.

## MIKEY-003 — App Shell

**Status:** complete.
**Physical acceptance:** PASS 2026-09-18.

Delivered and physically accepted:

- child-facing home menu;
- activity registry and carousel selection;
- PLAY / LEARN / CHALLENGE mode menu;
- CrowPanel hardware adapter;
- logical input layer;
- Jackpot PLAY migration into the shared shell;
- long-press Home navigation.

The accepted MIKEY-002 baseline file remains unchanged as a regression reference.

## MIKEY-004 — Reaction Racer

**Status:** complete.
**Physical acceptance:** PASS 2026-09-18.

Learning target: time and measurement.

Delivered and physically accepted:

- Home title refined with large `MIKEY` beneath `TECH LAB`;
- Reaction promoted to Activity 02 READY;
- Reaction PLAY added;
- randomized non-blocking GO timing;
- false-start detection;
- millisecond reaction measurement;
- session-best tracking;
- simultaneous screen/LED GO cue;
- generic Activity execution proven with a second activity;
- generalized mode selector;
- Jackpot regression passed.

The accepted MIKEY-002 baseline file remains unchanged as a regression reference.

## MIKEY-005 — Simon Lights

**Status:** next.

Implement Activity 03.

Learning target: memory and sequences.

## MIKEY-006 — Digital Dice

Implement Activity 04.

Learning target: randomness and probability.

## MIKEY-007 — Binary Lab

Implement Activity 05.

Learning target: bits and data representation.

## MIKEY-008 — Logic Lab

Implement Activity 06.

Learning target: conditions and Boolean logic.

## MIKEY-009 — Debug Detective

Implement Activity 07 and intentionally reuse faults from earlier activities.

Learning target: expected versus observed behavior, hypothesis, and evidence.

## MIKEY-010 — Phase 1 Review

Evaluate what Mikey can explain and demonstrate. Do not graduate to IDE lessons solely because all activities exist.

Possible readiness evidence:

- explains input/output using the device;
- predicts a simple timed or conditional outcome;
- explains that a sequence is stored information;
- reads small binary values with help or independently;
- uses AND/OR/NOT in ordinary-language rules;
- describes a bug as a mismatch between intended and actual behavior;
- proposes a simple debugging test.

## Later roadmap

Only after Phase 1 is stable:

- pixels/RGB/coordinates;
- sensors and analog measurement;
- device-to-device ESP32 communication;
- Wi-Fi/Bluetooth concepts;
- code reveal and small IDE modifications;
- eventually, guided creation of new activities.
