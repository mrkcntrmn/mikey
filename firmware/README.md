# Firmware

The firmware directory contains the accepted MIKEY-002 hardware baseline and the MIKEY-003 Mikey Tech Lab application shell.

## Jackpot baseline

`jackpot-baseline/JackpotBaseline/JackpotBaseline.ino` is the physically accepted MIKEY-002 CrowPanel target.

**Accepted:** 2026-09-18 at SHA `fc1735a0e6c34bec0caf89e5f03eac8317549f5a`.

Keep this sketch unchanged. It is the hardware oracle used to isolate regressions while the app shell evolves.

See [`jackpot-baseline/README.md`](jackpot-baseline/README.md) for Arduino settings and the completed acceptance checklist.

## Mikey Tech Lab app shell

`mikey-tech-lab/MikeyTechLab/` is the multi-activity application:

```text
MikeyTechLab/
├── MikeyTechLab.ino
└── src/
    ├── core/          # Activity contracts, registry, app state, input events
    ├── hardware/      # CrowPanel adapter (pins, display, touch, LEDs, encoder)
    ├── ui/            # Home menu + mode menu
    └── activities/    # Jackpot PLAY migration (others arrive later)
```

Boot flow:

```text
HOME → MODE SELECT → ACTIVITY
```

Long-press the encoder (~900 ms) to return Home from Mode Select or Jackpot.

## Boundary rule

An activity should ask for concepts such as:

- draw text;
- illuminate logical light N;
- read logical input events;
- get elapsed milliseconds;

It should not need to know which GPIO or controller library makes that happen.

MIKEY-002 keeps the standalone baseline board-specific. MIKEY-003 performs the abstraction in the CrowPanel adapter while preserving the accepted baseline file.
