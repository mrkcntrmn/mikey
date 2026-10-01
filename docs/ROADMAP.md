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
**Physical acceptance:** PASS 2026-09-18.

Accepted source baseline:

```text
fc1735a0e6c34bec0caf89e5f03eac8317549f5a
```

The accepted standalone sketch remains immutable as the hardware oracle while later milestones evolve the shared app shell.

## MIKEY-003 — App Shell

**Status:** complete.
**Physical acceptance:** PASS 2026-09-18.

Delivered:

- child-facing home menu;
- activity registry and carousel selection;
- PLAY / LEARN / CHALLENGE mode menu;
- CrowPanel hardware adapter;
- logical input layer;
- Jackpot PLAY migration into the shared shell;
- long-press Home navigation.

## MIKEY-004 — Reaction Racer

**Status:** complete.
**Physical acceptance:** PASS 2026-09-18.

Learning target: time and measurement.

Delivered:

- Reaction PLAY;
- randomized non-blocking GO timing;
- false-start detection;
- millisecond reaction measurement;
- session-best tracking;
- simultaneous screen/LED GO cue;
- Jackpot regression.

## MIKEY-005 — Simon Lights

**Status:** exact-head CI PASS; physical acceptance pending.

Learning target: memory and sequences.

Source candidate includes:

- Activity 03 PLAY READY;
- five-symbol sequence playback;
- encoder selection;
- press/tap submission;
- expected-versus-selected mismatch feedback;
- same-sequence retry;
- 16-symbol cap;
- native sequence tests;
- LEARN / CHALLENGE gated as SOON.

Dependency gate: physical CrowPanel acceptance and regressions before merge.

## MIKEY-006 — Digital Dice

**Status:** source-qualified; stacked on MIKEY-005.

Source-qualified head:

```text
756de4da985d8154f4a65e3ae04b456e7926e00f
```

Learning target: randomness and probability.

Source candidate includes:

- Activity 04 PLAY READY;
- non-blocking roll animation;
- six die faces;
- pip rendering;
- per-face LED cues;
- session roll count and observed frequency;
- tested random-index → face mapping;
- native Dice session tests;
- LEARN / CHALLENGE gated as SOON.

Dependency gate: after MIKEY-005 physical acceptance/merge, rebase or retarget MIKEY-006 onto the accepted baseline, rerun exact-head CI, then perform physical acceptance.

## MIKEY-007 — Binary Lab

**Status:** source candidate development; stacked on MIKEY-006.

Learning target: bits and data representation.

Source candidate delivers:

- Activity 05 PLAY READY;
- 0–15 four-bit range;
- explicit **8, 4, 2, 1** place-value ordering;
- encoder clockwise/counter-clockwise exploration;
- press/tap advances by one;
- wraparound at 0 and 15;
- decimal and four-bit text shown together;
- four on-screen bit cells;
- first four physical LEDs mirror the bits;
- fifth LED intentionally remains off;
- hardware-independent `BinaryValue` model;
- native tests for place values, known patterns, wrapping, and four-bit masking;
- LEARN / CHALLENGE remain gated as SOON.

Source qualification criteria:

- Binary native logic tests pass;
- Dice native logic tests remain green;
- Simon native logic tests remain green;
- Mikey Tech Lab Arduino compile passes;
- immutable Jackpot baseline compile passes;
- no GPIO ownership or network dependency is introduced.

Dependency gate:

- do not merge MIKEY-007 before MIKEY-005 and MIKEY-006 are accepted in order;
- after preceding milestones merge, rebase/retarget MIKEY-007 onto accepted `main`;
- rerun exact-head CI;
- physical acceptance must use the exact post-rebase SHA.

Planned physical exit criteria:

- Home shows `05 BINARY READY`;
- BINARY → PLAY opens with decimal 0 / binary 0000;
- clockwise rotation advances values;
- counter-clockwise rotation reverses values;
- 15 wraps to 0 and 0 wraps to 15;
- press and tap each advance exactly one value;
- screen binary text agrees with the four on-screen bit cells;
- first four physical LEDs agree with the displayed bits;
- fifth LED stays off;
- known checks pass: 5 = 0101, 10 = 1010, 15 = 1111;
- long-press Home exits cleanly;
- re-entry resets to 0;
- Jackpot, Reaction Racer, Simon, and Dice regressions pass.

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
