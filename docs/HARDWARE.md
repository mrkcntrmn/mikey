# Hardware

## Current known capabilities

The current learning prototype uses an ESP32-family development module with built-in visual/input features sufficient for the existing jackpot experiment: a screen, controllable lights/LEDs, and a screen/button-style interaction used to stop the wheel.

The prior prototype work also involved touch/display and LED library setup. Exact component models, pins, board selection, and dependency versions are **not yet recorded in this repository**.

## Do not guess hardware facts

Before firmware migration, capture the exact working configuration from the development machine and physical module.

Required MIKEY-002 capture:

| Item | Status |
|---|---|
| Development module manufacturer/model | TODO |
| ESP32 variant | TODO |
| Arduino board selection | TODO |
| ESP32 Arduino core version | TODO |
| Display controller | TODO |
| Display resolution/orientation | TODO |
| Touch controller/input mechanism | TODO |
| Built-in LED type/count | TODO |
| LED data/power behavior | TODO |
| Verified pin map | TODO |
| Required libraries and versions | TODO |
| Known-good jackpot sketch commit | TODO |

## Verification rule

A hardware value becomes canonical only after it is checked against at least one of:

1. the physical module/manufacturer documentation;
2. a known-good compiling/running sketch;
3. the installed board/library configuration that produced the known-good behavior.

## Architecture direction

Keep educational activity logic independent from physical hardware where practical.

Target layers:

```text
Activity / curriculum logic
        ↓
Mikey hardware interface
        ↓
ESP32 board adapter
        ↓
Display / touch / LEDs / sensors / radio
```

This avoids rewriting every activity if the project later moves to a different ESP32 display module.

## Expansion order

Do not add hardware merely because the ESP32 supports it. Add hardware when it creates a clear learning experience.

Likely progression:

1. built-in display/lights/input;
2. simple external button or potentiometer if useful;
3. one sensor at a time;
4. second ESP32 for device-to-device communication;
5. Wi-Fi/Bluetooth lessons after offline concepts are established.
