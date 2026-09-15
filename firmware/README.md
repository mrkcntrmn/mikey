# Firmware

The firmware directory will contain the device-side Mikey Tech Lab application.

## Current state

Only hardware-independent contracts are introduced in MIKEY-001. The existing working jackpot sketch has not yet been imported because the repository does not contain the exact validated board configuration, pin map, or dependency versions.

That is intentional.

## Target architecture

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

An activity should ask for concepts such as:

- draw text;
- illuminate logical light N;
- read the primary action input;
- get elapsed milliseconds;
- generate a bounded random value;

It should not need to know which GPIO or controller library makes that happen.

## Next step

See `docs/ROADMAP.md` — MIKEY-002 captures the exact board and migrates Jackpot as the known-good behavioral baseline.
