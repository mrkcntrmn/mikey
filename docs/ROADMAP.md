# Roadmap

## MIKEY-001 — Foundation

**Status:** implemented in repository foundation branch.

Goals:

- define the project vision;
- establish concept-before-syntax learning philosophy;
- define Core Seven curriculum;
- standardize PLAY / LEARN / CHALLENGE;
- standardize PREDICT → ACT → OBSERVE → EXPLAIN;
- establish a hardware abstraction boundary;
- avoid inventing unverified board details.

Exit criteria:

- documentation is internally consistent;
- future activities have a reusable specification;
- firmware contracts do not depend on a specific display/touch/LED library.

## MIKEY-002 — Hardware Capture + Jackpot Baseline

Goals:

1. Capture exact development module/model and ESP32 variant.
2. Capture board selection, Arduino core, libraries, and versions.
3. Capture verified display/touch/LED pin map.
4. Import the last known-good jackpot sketch without behavioral refactoring.
5. Build successfully from documented dependencies.
6. Flash and verify wheel direction, speed range/display, stop input, jackpot position, and light behavior.
7. Tag the reproducible baseline before refactoring.

Important rule: **baseline first, refactor second**.

## MIKEY-003 — App Shell

Goals:

- child-facing home menu;
- activity selection;
- PLAY / LEARN / CHALLENGE mode selection;
- shared input/display/light adapters;
- settings persistence only where it improves the experience;
- preserve Jackpot as Activity 01.

## MIKEY-004 — Reaction Racer

Implement Activity 02 and use it to prove that a second activity can share the app shell and hardware adapters cleanly.

Learning target: time and measurement.

## MIKEY-005 — Simon Lights

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
