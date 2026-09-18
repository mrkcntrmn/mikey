# Hardware

## Target development module

MIKEY-002 targets the **Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display (240×240 IPS Round Touch Knob Screen)**.

This identification is supported by the prior prototype dependency fingerprints (`CST816D.h` and `Adafruit_NeoPixel.h`) and Elecrow's published hardware/example configuration. Physical-device acceptance completed **2026-09-18** against source SHA `fc1735a0e6c34bec0caf89e5f03eac8317549f5a`.

## Hardware profile

| Component | Configuration |
|---|---|
| MCU | ESP32-S3 / ESP32-S3R8 family |
| Flash | 16 MB |
| PSRAM | 8 MB, OPI |
| Main display | 1.28-inch 240×240 IPS |
| Display controller | GC9A01 / GC9A01A |
| Touch controller | CST816D, I2C address `0x15` |
| Built-in RGB LEDs | 5 × WS2812 |
| Rotary encoder | A + B + push switch |

## Verified vendor pin map

| Function | GPIO |
|---|---:|
| Display SCLK | 10 |
| Display MOSI | 11 |
| Display MISO | not connected |
| Display DC | 3 |
| Display CS | 9 |
| Display RESET | 14 |
| Screen backlight | 46 |
| Touch SDA | 6 |
| Touch SCL | 7 |
| Touch INT | 5 |
| Touch RESET | 13 |
| WS2812 data | 48 |
| WS2812 count | 5 |
| Encoder A | 45 |
| Encoder B | 42 |
| Encoder switch | 41 |
| Power indicator | 40 |
| Display/power enable rails used by vendor example | 1 and 2 |

## Arduino upload configuration

Use:

- Board: **ESP32S3 Dev Module**
- Flash Size: **16MB (128Mb)**
- Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)** for the small Jackpot baseline; Elecrow's newer LVGL example also provides an `elecrow_s3` partition for its larger UI package.
- PSRAM: **OPI PSRAM**

FQBN used for physical upload:

```text
esp32:esp32:esp32s3:FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi
```

### Pinned development dependencies

The Jackpot baseline deliberately avoids LVGL and the external CST816D library. It needs only:

- ESP32 Arduino Core **3.3.8**
- LovyanGFX **1.2.26**
- Adafruit NeoPixel **1.15.1**

Touch polling for the CST816D is implemented directly through I2C with `Wire1`, which removes the earlier `CST816D.h: No such file or directory` failure mode.

## Source provenance

Hardware values are based on Elecrow's current product wiki, Arduino guide, and official example repository:

- https://elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html
- https://www.elecrow.com/wiki/1.28_Arduino_LVGL_Rotary_Guide.html
- https://github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen

## Physical-device acceptance

MIKEY-002 physical acceptance completed **2026-09-18** on the CrowPanel. Verified:

- display renders upright;
- wheel advances clockwise on screen;
- the top screen position is the jackpot position;
- touch toggles RUNNING/STOPPED once per press;
- encoder clockwise increases speed and counter-clockwise decreases it;
- speed is constrained to 25–1000%;
- all five built-in LEDs participate in the chase;
- physical LED chase direction feels consistent with the screen wheel;
- no spontaneous resets occur during sustained 1000% play.

Accepted source baseline:

```text
fc1735a0e6c34bec0caf89e5f03eac8317549f5a
```

That standalone sketch remains the immutable hardware oracle. MIKEY-003 migrates the same pin map and behavior into the shared CrowPanel adapter without changing the accepted baseline file.
