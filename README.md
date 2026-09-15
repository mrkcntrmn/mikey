# Mikey Tech Lab

Mikey Tech Lab is an ESP32-first technology learning project designed for Mikey.

The goal is **technology literacy before programming syntax**. The ESP32 is used as a small interactive science and computing lab: lights, a display, touch/buttons, timers, memory, randomness, sensors, and later wireless communication. Arduino IDE instruction comes later, after the ideas behind the code are familiar.

## Home screen

The target device experience is a single **Mikey Tech Lab** home screen that grows as activities are added:

```text
MIKEY TECH LAB

🎰 Jackpot
⚡ Reaction
🧠 Simon
🎲 Dice
🔢 Binary
🚦 Logic
🐞 Debug
```

Selecting an activity leads into its **PLAY**, **LEARN**, or **CHALLENGE** experience. The current MIKEY-002 firmware boots directly into Jackpot while we qualify the physical hardware; MIKEY-003 introduces the shared home screen and activity launcher.

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

MIKEY-001 established the curriculum and application contracts. MIKEY-002 adds the first board-specific, uploadable Jackpot baseline for the Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display.

The Jackpot baseline intentionally stays separate from the future multi-activity app shell until it compiles in CI and is accepted on the physical device.

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
        └── src/
            ├── Activity.h
            └── LearningLoop.h
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

**MIKEY-002 — Hardware Capture + Jackpot Baseline**

1. Compile the baseline against the pinned Arduino dependencies.
2. Upload it to the physical CrowPanel.
3. Verify display orientation, clockwise wheel direction, touch stop/start, encoder speed control, 25–1000% range, and the five built-in LEDs.
4. Record any physical-device correction as evidence.
5. Freeze the accepted baseline before MIKEY-003 refactors it into the app shell.
