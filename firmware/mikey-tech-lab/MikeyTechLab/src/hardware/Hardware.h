#pragma once

#include <stdint.h>

#include "../core/InputEvent.h"

namespace mikey {

// Shared RGB565 palette used by menus and Jackpot.
static constexpr uint16_t kColorBlack = 0x0000;
static constexpr uint16_t kColorWhite = 0xFFFF;
static constexpr uint16_t kColorGray = 0x4208;
static constexpr uint16_t kColorYellow = 0xFFE0;
static constexpr uint16_t kColorCyan = 0x07FF;
static constexpr uint16_t kColorGreen = 0x07E0;
static constexpr uint16_t kColorRed = 0xF800;

static constexpr int kDisplaySize = 240;
static constexpr int kLedCountLogical = 5;
static constexpr uint32_t kLongPressMs = 900;

// Logical drawing / LED surface used by menus and activities.
// Board-specific GPIO and controllers stay inside CrowPanelHardware.
class Hardware {
 public:
  virtual ~Hardware() = default;

  virtual void begin() = 0;
  virtual uint32_t nowMs() const = 0;

  virtual InputFrame pollInput() = 0;

  virtual void clearFrame(uint16_t color = kColorBlack) = 0;
  virtual void fillCircle(int16_t x, int16_t y, int16_t radius, uint16_t color) = 0;
  virtual void drawCircle(int16_t x, int16_t y, int16_t radius, uint16_t color) = 0;
  virtual void drawCenteredText(const char* text, int16_t centerY, uint8_t textSize,
                                uint16_t color) = 0;
  virtual void presentFrame() = 0;

  virtual void clearLeds() = 0;
  virtual void setLed(int index, uint8_t r, uint8_t g, uint8_t b) = 0;
  virtual void showLeds() = 0;
};

}  // namespace mikey
