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
    └── activities/    # Jackpot + Reaction + Simon + Dice + Binary PLAY
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

Binary Lab is intentionally small and visual:

- encoder clockwise increments the decimal value;
- encoder counter-clockwise decrements it;
- encoder short-press or screen tap advances by one;
- values wrap through 0–15;
- the display shows the decimal value and four-bit text;
- four on-screen bit cells are ordered **8, 4, 2, 1**;
- the first four physical LEDs mirror those four bits;
- the fifth LED stays off so the four-bit representation is unambiguous.

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
