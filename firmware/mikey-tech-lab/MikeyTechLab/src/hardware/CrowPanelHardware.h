#pragma once

#define LGFX_USE_V1

#include <Adafruit_NeoPixel.h>
#include <LovyanGFX.hpp>
#include <Wire.h>

#include "Hardware.h"

namespace mikey {

class CrowPanelHardware : public Hardware {
 public:
  CrowPanelHardware();

  void begin() override;
  uint32_t nowMs() const override;
  InputFrame pollInput() override;

  void clearFrame(uint16_t color = kColorBlack) override;
  void fillCircle(int16_t x, int16_t y, int16_t radius, uint16_t color) override;
  void drawCircle(int16_t x, int16_t y, int16_t radius, uint16_t color) override;
  void drawCenteredText(const char* text, int16_t centerY, uint8_t textSize,
                        uint16_t color) override;
  void presentFrame() override;

  void clearLeds() override;
  void setLed(int index, uint8_t r, uint8_t g, uint8_t b) override;
  void showLeds() override;

 private:
  class DisplayDriver : public lgfx::LGFX_Device {
   public:
    DisplayDriver();

   private:
    lgfx::Panel_GC9A01 panel_;
    lgfx::Bus_SPI bus_;
  };

  class TouchReader {
   public:
    void begin();
    bool touched(uint16_t* x = nullptr, uint16_t* y = nullptr);

   private:
    static constexpr uint8_t kAddress = 0x15;
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t* value);
    bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length);
  };

  // Accepted CrowPanel pin map from MIKEY-002.
  static constexpr int kTouchSdaPin = 6;
  static constexpr int kTouchSclPin = 7;
  static constexpr int kTouchIntPin = 5;
  static constexpr int kTouchResetPin = 13;
  static constexpr int kLedPin = 48;
  static constexpr int kLedCount = 5;
  static constexpr int kEncoderAPin = 45;
  static constexpr int kEncoderBPin = 42;
  static constexpr int kEncoderSwitchPin = 41;
  static constexpr int kBacklightPin = 46;
  static constexpr int kPowerLightPin = 40;
  static constexpr int kPowerEnablePin1 = 1;
  static constexpr int kPowerEnablePin2 = 2;

  DisplayDriver display_;
  LGFX_Sprite canvas_;
  Adafruit_NeoPixel pixels_;
  TouchReader touch_;

  int lastEncoderA_ = HIGH;
  bool lastEncoderButton_ = HIGH;
  bool buttonDown_ = false;
  uint32_t buttonDownAtMs_ = 0;
  bool lastTouchState_ = false;
  uint32_t lastTouchPollMs_ = 0;
  uint32_t lastTouchEdgeMs_ = 0;
  uint32_t lastButtonReleaseMs_ = 0;
};

}  // namespace mikey
