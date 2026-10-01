# Firmware

The firmware directory contains the accepted MIKEY-002 hardware baseline and the Mikey Tech Lab multi-activity application.

## Jackpot baseline

`jackpot-baseline/JackpotBaseline/JackpotBaseline.ino` is the physically accepted MIKEY-002 CrowPanel target.

**Accepted:** 2026-09-18 at SHA `fc1735a0e6c34bec0caf89e5f03eac8317549f5a`.

Keep this sketch unchanged. It is the hardware oracle used to isolate regressions while the app shell evolves.

See [`jackpot-baseline/README.md`](jackpot-baseline/README.md) for Arduino settings and the completed acceptance checklist.

## Mikey Tech Lab app

`mikey-tech-lab/MikeyTechLab/` is the multi-activity application:

```text
MikeyTechLab/
├── MikeyTechLab.ino
└── src/
    ├── core/          # Activity contracts, registry, app state, input events
    ├── hardware/      # CrowPanel adapter (pins, display, touch, LEDs, encoder, RNG)
    ├── ui/            # Home menu + mode menu
    └── activities/    # Jackpot + Reaction Racer + Simon + Dice + Binary PLAY
```

Boot flow:

```text
HOME → MODE SELECT → ACTIVITY
```

Current PLAY-ready activities in the MIKEY-007 stacked candidate:

```text
01 JACKPOT   PLAY READY
02 REACTION  PLAY READY
03 SIMON     PLAY READY
04 DICE      PLAY READY
05 BINARY    PLAY READY
```

All five activities share:

- the Activity contract;
- the logical input-event model;
- the CrowPanel hardware adapter;
- the Home carousel;
- the mode selector;
- the generic activity runtime.

Simon Lights uses the five logical LEDs as five ordered symbols. During input, the encoder selects a light and encoder short-press or screen tap submits it. Correct rounds append one new symbol. A mismatch identifies the expected and selected light, then lets the learner replay the same sequence.

Digital Dice uses screen tap or encoder short-press to start a non-blocking roll animation. The final result is one of six die faces. The screen shows standard pip patterns, roll number, and how many times the current face has appeared during the session. LED color changes with the face as a physical output cue. The displayed session frequency is observational; PLAY does not claim that a short run must look evenly distributed.

Binary Lab explores values 0–15 as four bits. The display shows the decimal value, binary text, and four bit cells ordered **8, 4, 2, 1**. The first four physical LEDs mirror those bits; the fifth LED stays off so the four-bit representation is unambiguous. Rotate to explore values or press/tap to advance by one.

LEARN and CHALLENGE remain gated as SOON for all READY activities.

Long-press the encoder (~900 ms) to return Home from Mode Select or an active activity.

## Boundary rule

An activity should ask for concepts such as:

- draw text;
- illuminate logical light N;
- read logical input events;
- get elapsed milliseconds;
- draw a bounded random value;

It should not need to know which GPIO or controller library makes that happen.

Deterministic learning rules should remain hardware-independent where practical so native CI can test them. Physical acceptance is still required for integrated display, LED, touch/encoder, timing, and navigation behavior.

MIKEY-002 keeps the standalone baseline board-specific. The CrowPanel adapter performs the abstraction while preserving the accepted baseline file.
