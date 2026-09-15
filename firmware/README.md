# Firmware

The firmware directory contains both reproducible hardware baselines and the hardware-independent Mikey Tech Lab application contracts.

## Jackpot baseline

`jackpot-baseline/JackpotBaseline/JackpotBaseline.ino` is the first uploadable board-specific target.

It exists to prove the physical Elecrow board configuration before the game is refactored into the multi-activity app shell.

See [`jackpot-baseline/README.md`](jackpot-baseline/README.md) for exact Arduino settings and the physical acceptance checklist.

## Target app architecture

```text
Mikey Tech Lab app
├── activity registry
├── learning mode / learning loop
├── shared settings/progress
├── activities
│   ├── jackpot
│   ├── reaction
│   ├── simon
│   ├── dice
│   ├── binary
│   ├── logic
│   └── debugging
└── hardware interface
    └── board-specific ESP32 adapter
```

## Boundary rule

An activity should eventually ask for concepts such as:

- draw text;
- illuminate logical light N;
- read the primary action input;
- get elapsed milliseconds;
- generate a bounded random value;

It should not need to know which GPIO or controller library makes that happen.

MIKEY-002 intentionally keeps the standalone baseline board-specific. MIKEY-003 performs the abstraction only after the device behavior is accepted.
