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

**Status:** source candidate; exact-head CI PASS; physical acceptance pending.

Learning target: memory and sequences.

Source candidate delivers:

- Activity 03 promoted to PLAY READY;
- five-symbol sequence playback using the five logical LEDs;
- non-blocking playback timing;
- encoder rotation to choose a light;
- encoder short-press or screen tap to submit the selected light;
- ordered input checking;
- one-symbol sequence growth after each successful round;
- expected-versus-selected feedback on mistakes;
- same-sequence replay after a mistake;
- long-press Home support;
- bounded 16-symbol session target;
- hardware-independent Simon sequence tests;
- LEARN and CHALLENGE remain gated as SOON.

Physical exit criteria:

- firmware uploads to the physical device;
- all five logical lights are visually distinguishable on screen and LEDs;
- playback order on screen matches LED order;
- encoder selection wraps through all five lights in the expected direction;
- press and touch both submit the selected light;
- correct sequences advance exactly one symbol;
- incorrect input reports the mismatch and replays the same sequence;
- long-press returns Home without accidental submission;
- leaving and re-entering Simon starts a clean session;
- Jackpot regression passes;
- Reaction Racer regression passes.

MIKEY-005 must pass this gate before it is merged.

## MIKEY-006 — Digital Dice

**Status:** source candidate development; stacked on MIKEY-005.

Learning target: randomness and probability.

Source candidate delivers:

- Activity 04 promoted to PLAY READY;
- screen tap or encoder short-press starts a roll;
- non-blocking nine-frame rolling animation;
- six standard die-face outcomes;
- standard pip layout on the round display;
- distinct LED color cue for each outcome;
- session roll counter;
- per-face observed frequency counter for the current result;
- hardware-independent session/counting tests;
- long-press Home support;
- LEARN and CHALLENGE remain gated as SOON.

Source qualification criteria:

- Dice session tests pass;
- Simon sequence tests continue to pass;
- app-shell firmware compiles;
- immutable Jackpot baseline compiles;
- MIKEY-006 diff contains no board-pin ownership or cloud/network dependency.

Dependency gate:

- do not merge MIKEY-006 before MIKEY-005 physical acceptance and merge;
- after MIKEY-005 merges, rebase or retarget MIKEY-006 onto the accepted baseline and rerun exact-head CI;
- physical acceptance must use that exact post-rebase MIKEY-006 SHA.

Planned physical exit criteria:

- Home shows `04 DICE READY`;
- DICE → PLAY opens the ready screen;
- encoder short-press starts exactly one roll;
- screen tap starts exactly one roll;
- rolling animation stays responsive and does not block long-press Home;
- final result is always 1 through 6;
- each face renders the correct pip pattern;
- result LED cue is visible and stable until the next roll;
- roll number increases by exactly one per completed roll;
- current-face frequency increases only when that face is rolled;
- leaving and re-entering Digital Dice starts a clean session;
- Jackpot, Reaction Racer, and Simon regressions pass.

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
