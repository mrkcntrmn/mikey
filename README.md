# Mikey Tech Lab

Mikey Tech Lab is an ESP32-first technology learning project designed for Mikey, age 7.

The goal is **technology literacy before programming syntax**. The ESP32 is used as a small interactive science and computing lab: lights, a display, touch/buttons, timers, memory, randomness, sensors, and later wireless communication. Arduino IDE instruction comes later, after the ideas behind the code are familiar.

## Learning model

Every activity should favor this loop:

**PREDICT → ACT → OBSERVE → EXPLAIN**

Whenever practical, Mikey predicts what the machine will do before the result is revealed.

Activities expose three modes:

- **PLAY** — enjoy the activity with minimal interruption.
- **LEARN** — surface one technology concept at a time.
- **CHALLENGE** — solve, predict, diagnose, or explain before advancing.

## Phase 1: Core Seven

| # | Activity | Primary concept |
|---|---|---|
| 1 | Jackpot Wheel | Input → process → output, timing, state |
| 2 | Reaction Racer | Time and measurement |
| 3 | Simon Lights | Memory and sequences |
| 4 | Digital Dice | Randomness and probability |
| 5 | Binary Lab | Bits and data representation |
| 6 | Logic Lab | Conditions and Boolean logic |
| 7 | Debug Detective | Debugging and engineering reasoning |

The progression is intentionally conceptual:

**CONTROL → MEASURE → REMEMBER → GENERATE → REPRESENT → DECIDE → DEBUG**

## Current status

The GitHub repository was created with only a placeholder README. The working ESP32 jackpot experiment exists outside this repository and has not yet been migrated with a verified hardware profile. This foundation therefore does **not** guess the board pin map, display driver, touch driver, LED driver, or library versions.

MIKEY-002 will capture the exact development module configuration and migrate the validated jackpot sketch as Activity 01.

## Repository layout

```text
mikey/
├── README.md
├── docs/
│   ├── VISION.md
│   ├── LEARNING-PHILOSOPHY.md
│   ├── CURRICULUM.md
│   ├── ACTIVITY-SPEC.md
│   ├── HARDWARE.md
│   ├── MENTOR-GUIDE.md
│   ├── ROADMAP.md
│   └── DECISIONS.md
└── firmware/
    ├── README.md
    └── mikey-tech-lab/
        └── src/
            ├── Activity.h
            └── LearningLoop.h
```

## Design constraints

- Age-appropriate and adult-guided.
- Offline-first in the early curriculum.
- No account, personal-data collection, ads, or cloud dependency in Phase 1.
- One primary concept per lesson, even when an activity demonstrates several.
- Prefer physical cause-and-effect over explanation alone.
- Do not require typing code to succeed in Phase 1.
- Do not hide mistakes; debugging is a first-class activity.
- Hardware-specific claims must be verified before they are committed as facts.

## Documentation

Start with [Vision](docs/VISION.md), [Learning Philosophy](docs/LEARNING-PHILOSOPHY.md), and the [Curriculum](docs/CURRICULUM.md). New games should follow [Activity Spec](docs/ACTIVITY-SPEC.md).

## Near-term milestone

**MIKEY-002 — Hardware Capture + Jackpot Migration**

1. Record exact ESP32 development module/model.
2. Record display, touch/button, LED hardware and verified pins.
3. Record Arduino core, board selection, and required libraries/versions.
4. Import the last known-good jackpot sketch unchanged as a baseline.
5. Verify build and device behavior.
6. Refactor only after the baseline is reproducible.
