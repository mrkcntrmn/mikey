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
03 SIMON      READY
04 DICE       READY
05 BINARY     READY
06 LOGIC      READY
07 DEBUG      READY
```

All seven **PLAY** activities are integrated on `main` as of 2026-10-15 (Core Seven firmware integration SHA `e8fe7a202571c1de1c76d33fca93f041d68145f8`). The project owner reported physical acceptance PASS for MIKEY-005 through MIKEY-009; these are owner reports, not independently observed uploads. **LEARN and CHALLENGE remain SOON**. The MIKEY-002 Jackpot standalone baseline remains immutable.

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

MIKEY-005 Simon Lights, MIKEY-006 Digital Dice, MIKEY-007 Binary Lab, MIKEY-008 Logic Lab, and MIKEY-009 Debug Detective all passed their source CI gates and merged to `main` in dependency order on **2026-10-15**. The owner confirmed physical acceptance PASS for these activities on the same date. The accepted seven-game firmware tree matches the exact CI-qualified MIKEY-009 tree. For commit-by-commit provenance, acceptance limits and CI runs, see [Core Seven Closeout](docs/CORE-SEVEN-CLOSEOUT-2026-10-15.md).

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
│   ├── DECISIONS.md
│   ├── CORE-SEVEN-CLOSEOUT-2026-10-15.md
│   └── MIKEY-010-READINESS.md
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

## Optional remote firmware updates (MIKEY-011)

**Planning only; not installed or enabled.** The CrowPanel's accepted USB profile uses a `huge_app` **No OTA** partition scheme. The planned adult-controlled Wi-Fi upgrade path uses outbound HTTPS, authenticated/signed releases, and a qualified dual-slot OTA partition layout; it does **not** need Tailscale or a spare home computer. All seven PLAY activities must remain fully offline-capable and the accepted Jackpot baseline must remain unchanged. See [MIKEY-011 HTTPS OTA Plan](docs/MIKEY-011-HTTPS-OTA-PLAN.md) for board preflight, firmware-release security, canary testing and USB recovery gates.

## Documentation

Start with [Vision](docs/VISION.md), [Learning Philosophy](docs/LEARNING-PHILOSOPHY.md), and the [Curriculum](docs/CURRICULUM.md). New games should follow [Activity Spec](docs/ACTIVITY-SPEC.md).

For the physical board and upload configuration, see [Hardware](docs/HARDWARE.md) and [Jackpot Baseline](firmware/jackpot-baseline/README.md). For the accepted seven-game integration and next educational review, see [Core Seven Closeout](docs/CORE-SEVEN-CLOSEOUT-2026-10-15.md) and [MIKEY-010 Readiness](docs/MIKEY-010-READINESS.md).

## Current phase and gates

**Core Seven PLAY: source-integrated; owner-confirmed physical PASS.** PRs #5–#9 are merged; no additional PLAY implementation is awaiting promotion.

**MIKEY-010 — Phase 1 Review:** next phase. Use [MIKEY-010 Readiness](docs/MIKEY-010-READINESS.md) to observe actual concept understanding without making play feel like a test. Do not declare IDE readiness from game-completion counts alone.

**LEARN / CHALLENGE:** still SOON. Their interaction design and testable objectives are planned, not delivered, in [MIKEY-010 Readiness](docs/MIKEY-010-READINESS.md). Preserve PLAY behavior and the hardware adapter while implementing new modes in a separate future change.

**Provenance:** the 2026-10-15 owner statement did not include exact flashing logs or per-item test receipts; see the [Core Seven Closeout](docs/CORE-SEVEN-CLOSEOUT-2026-10-15.md). No independent physical verification is claimed.
