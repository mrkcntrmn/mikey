# Mikey Tech Lab

Mikey Tech Lab is an ESP32-first technology learning project designed for Mikey.

The goal is **technology literacy before programming syntax**. The ESP32 is used as a small interactive science and computing lab: lights, a display, touch/buttons, timers, memory, randomness, sensors, and later wireless communication. Arduino IDE instruction comes later, after the ideas behind the code are familiar.

## Home screen

The target device experience is a single **Mikey Tech Lab** home screen that grows as activities are added:

```text
TECH LAB
  MIKEY

01 JACKPOT    READY
02 REACTION   READY
03 SIMON      SOON
04 DICE       SOON
05 BINARY     SOON
06 LOGIC      SOON
07 DEBUG      SOON
```

Selecting an activity leads into its **PLAY**, **LEARN**, or **CHALLENGE** experience. MIKEY-004 completed Reaction Racer PLAY on the shared app shell. The accepted MIKEY-002 Jackpot baseline remains available as a standalone comparison target.

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

MIKEY-001 established the curriculum and application contracts. MIKEY-002 delivered and physically accepted the CrowPanel Jackpot baseline on **2026-09-18** (`fc1735a`). MIKEY-003 completed the shared home menu and app shell (physical acceptance **2026-09-18**). MIKEY-004 completed Reaction Racer PLAY and home title refinement (physical acceptance **2026-09-18**).

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
    ├── jackpot-baseline/
    │   ├── README.md
    │   └── JackpotBaseline/
    │       └── JackpotBaseline.ino
    └── mikey-tech-lab/
        └── MikeyTechLab/
            ├── MikeyTechLab.ino
            └── src/
```

## Design constraints

- Child-focused and adult-guided.
- Offline-first in the early curriculum.
- No account, personal-data collection, ads, or cloud dependency in Phase 1.
- One primary concept per lesson, even when an activity demonstrates several.
- Prefer physical cause-and-effect over explanation alone.
- Do not require typing code to succeed in Phase 1.
- Do not hide mistakes; debugging is a first-class activity.
- Hardware-specific claims must be verified before they are committed as facts.

## Documentation

Start with [Vision](docs/VISION.md), [Learning Philosophy](docs/LEARNING-PHILOSOPHY.md), and the [Curriculum](docs/CURRICULUM.md). New games should follow [Activity Spec](docs/ACTIVITY-SPEC.md).

For the physical board and upload configuration, see [Hardware](docs/HARDWARE.md) and [Jackpot Baseline](firmware/jackpot-baseline/README.md).

## Near-term milestone

**MIKEY-005 — Simon Lights**

1. Implement Activity 03 Simon Lights PLAY.
2. Teach memory and sequences on the shared app shell.
3. Keep LEARN / CHALLENGE and later activities explicitly gated as SOON.
4. Preserve the accepted MIKEY-002 baseline unchanged for regression comparison.
