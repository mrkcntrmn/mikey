# Jackpot Baseline

This is the MIKEY-002 physical-device baseline for the Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display.

## What it does

- Draws a 12-position wheel on the round display.
- Keeps the jackpot fixed at the top position.
- Advances the wheel clockwise.
- Chases the five built-in WS2812 LEDs with the wheel.
- Touch anywhere on the screen to stop; touch again to restart.
- Press the encoder switch to stop/restart as a second input path.
- Rotate the encoder to change speed in 25% steps.
- Constrains speed to **25%–1000%**.
- Shows the speed percentage on screen.
- Shows JACKPOT when stopped on the top position and MISS otherwise.

## Required software

Recommended/pinned baseline:

- Arduino IDE 2.3.x
- ESP32 Arduino Core 3.3.8
- LovyanGFX 1.2.26
- Adafruit NeoPixel 1.15.1

No LVGL installation is required for this baseline. No separate CST816D library is required.

## Arduino board settings

Under **Tools**:

- Board → `esp32` → **ESP32S3 Dev Module**
- Flash Size → **16MB (128Mb)**
- Partition Scheme → **Huge APP (3MB No OTA/1MB SPIFFS)**
- PSRAM → **OPI PSRAM**
- Port → the connected CrowPanel serial port

## Upload

1. Install the ESP32 board package and the two pinned libraries above.
2. Open `JackpotBaseline/JackpotBaseline.ino`.
3. Select the board settings above.
4. Connect the CrowPanel with a USB data cable.
5. Click **Verify** first.
6. Click **Upload**.
7. If the board does not enter download mode, follow Elecrow's boot-button procedure for this module and retry the upload.

## Controls

- **Touch screen:** stop / restart.
- **Encoder press:** stop / restart.
- **Encoder clockwise:** faster.
- **Encoder counter-clockwise:** slower.

## Acceptance checklist

After upload, verify:

- [ ] round display is upright and readable;
- [ ] screen wheel moves clockwise;
- [ ] jackpot stays at the top;
- [ ] touch stops exactly once per press;
- [ ] second touch restarts;
- [ ] knob press also toggles run/stop;
- [ ] clockwise knob rotation increases speed;
- [ ] counter-clockwise rotation decreases speed;
- [ ] speed reads from 25% through 1000%;
- [ ] five built-in LEDs chase while running;
- [ ] stopped LED position remains stable;
- [ ] jackpot result appears when stopped at the top;
- [ ] sustained 1000% operation does not reset or freeze.

## Tuning constants

At the top of the sketch:

- `kBaseStepMs` defines 100% speed.
- `kSpeedStepPercent` defines encoder increments.
- `kReversePhysicalLedOrder` controls the five-LED chase ordering.

The screen wheel index itself is defined clockwise and should not be reversed to compensate for a physical LED ordering difference.

## Provenance

Board configuration and pin mapping are based on Elecrow's official wiki and example repository. The sketch is a purpose-built minimal baseline rather than a copy of Elecrow's LVGL factory application.
