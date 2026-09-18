#include "CrowPanelHardware.h"

namespace mikey {

CrowPanelHardware::DisplayDriver::DisplayDriver() {
  {
    auto cfg = bus_.config();
    cfg.spi_host = SPI2_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 80000000;
    cfg.freq_read = 20000000;
    cfg.spi_3wire = true;
    cfg.use_lock = true;
    cfg.dma_channel = SPI_DMA_CH_AUTO;
    cfg.pin_sclk = 10;
    cfg.pin_mosi = 11;
    cfg.pin_miso = -1;
    cfg.pin_dc = 3;
    bus_.config(cfg);
    panel_.setBus(&bus_);
  }

  {
    auto cfg = panel_.config();
    cfg.pin_cs = 9;
    cfg.pin_rst = 14;
    cfg.pin_busy = -1;
    cfg.memory_width = 240;
    cfg.memory_height = 240;
    cfg.panel_width = 240;
    cfg.panel_height = 240;
    cfg.offset_x = 0;
    cfg.offset_y = 0;
    cfg.offset_rotation = 0;
    cfg.dummy_read_pixel = 8;
    cfg.dummy_read_bits = 1;
    cfg.readable = false;
    cfg.invert = true;
    cfg.rgb_order = false;
    cfg.dlen_16bit = false;
    cfg.bus_shared = false;
    panel_.config(cfg);
  }

  setPanel(&panel_);
}

void CrowPanelHardware::TouchReader::begin() {
  Wire1.begin(CrowPanelHardware::kTouchSdaPin, CrowPanelHardware::kTouchSclPin);
  Wire1.setClock(400000);

  pinMode(CrowPanelHardware::kTouchIntPin, OUTPUT);
  digitalWrite(CrowPanelHardware::kTouchIntPin, HIGH);
  delay(1);
  digitalWrite(CrowPanelHardware::kTouchIntPin, LOW);
  delay(1);

  pinMode(CrowPanelHardware::kTouchResetPin, OUTPUT);
  digitalWrite(CrowPanelHardware::kTouchResetPin, LOW);
  delay(10);
  digitalWrite(CrowPanelHardware::kTouchResetPin, HIGH);
  delay(300);

  writeRegister(0xFE, 0xFF);
  pinMode(CrowPanelHardware::kTouchIntPin, INPUT_PULLUP);
}

bool CrowPanelHardware::TouchReader::touched(uint16_t* x, uint16_t* y) {
  uint8_t fingerCount = 0;
  if (!readRegister(0x02, &fingerCount) || fingerCount == 0) {
    return false;
  }

  uint8_t coordinates[4] = {0, 0, 0, 0};
  if (!readRegisters(0x03, coordinates, sizeof(coordinates))) {
    return false;
  }

  if (x) *x = ((coordinates[0] & 0x0F) << 8) | coordinates[1];
  if (y) *y = ((coordinates[2] & 0x0F) << 8) | coordinates[3];
  return true;
}

bool CrowPanelHardware::TouchReader::writeRegister(uint8_t reg, uint8_t value) {
  Wire1.beginTransmission(kAddress);
  Wire1.write(reg);
  Wire1.write(value);
  return Wire1.endTransmission() == 0;
}

bool CrowPanelHardware::TouchReader::readRegister(uint8_t reg, uint8_t* value) {
  Wire1.beginTransmission(kAddress);
  Wire1.write(reg);
  if (Wire1.endTransmission(false) != 0) return false;
  if (Wire1.requestFrom(kAddress, static_cast<uint8_t>(1)) != 1) return false;
  *value = Wire1.read();
  return true;
}

bool CrowPanelHardware::TouchReader::readRegisters(uint8_t reg, uint8_t* buffer,
                                                   size_t length) {
  Wire1.beginTransmission(kAddress);
  Wire1.write(reg);
  if (Wire1.endTransmission(false) != 0) return false;

  size_t received = Wire1.requestFrom(kAddress, static_cast<uint8_t>(length));
  if (received != length) return false;

  for (size_t i = 0; i < length; ++i) {
    buffer[i] = Wire1.read();
  }
  return true;
}

CrowPanelHardware::CrowPanelHardware()
    : canvas_(&display_),
      pixels_(kLedCount, kLedPin, NEO_GRB + NEO_KHZ800) {}

void CrowPanelHardware::begin() {
  Serial.begin(115200);
  delay(50);

  pinMode(kPowerLightPin, OUTPUT);
  digitalWrite(kPowerLightPin, LOW);
  pinMode(kPowerEnablePin1, OUTPUT);
  digitalWrite(kPowerEnablePin1, HIGH);
  pinMode(kPowerEnablePin2, OUTPUT);
  digitalWrite(kPowerEnablePin2, HIGH);

  touch_.begin();

  display_.init();
  display_.setRotation(0);

  pinMode(kBacklightPin, OUTPUT);
  digitalWrite(kBacklightPin, HIGH);

  canvas_.setColorDepth(16);
  canvas_.createSprite(kDisplaySize, kDisplaySize);

  pixels_.begin();
  pixels_.setBrightness(40);
  pixels_.clear();
  pixels_.show();

  pinMode(kEncoderAPin, INPUT_PULLUP);
  pinMode(kEncoderBPin, INPUT_PULLUP);
  pinMode(kEncoderSwitchPin, INPUT_PULLUP);
  lastEncoderA_ = digitalRead(kEncoderAPin);
  lastEncoderButton_ = digitalRead(kEncoderSwitchPin);
}

uint32_t CrowPanelHardware::nowMs() const { return millis(); }

InputFrame CrowPanelHardware::pollInput() {
  InputFrame frame;

  const int encoderA = digitalRead(kEncoderAPin);
  if (encoderA != lastEncoderA_ && encoderA == HIGH) {
    const int encoderB = digitalRead(kEncoderBPin);
    if (encoderB != encoderA) {
      frame.encoderClockwise = true;
    } else {
      frame.encoderCounterClockwise = true;
    }
  }
  lastEncoderA_ = encoderA;

  const bool buttonNow = digitalRead(kEncoderSwitchPin) == LOW;
  const uint32_t now = millis();
  if (buttonNow && !buttonDown_) {
    buttonDown_ = true;
    buttonDownAtMs_ = now;
  } else if (!buttonNow && buttonDown_) {
    buttonDown_ = false;
    const uint32_t heldMs = now - buttonDownAtMs_;
    if (now - lastButtonReleaseMs_ >= 180) {
      lastButtonReleaseMs_ = now;
      if (heldMs >= kLongPressMs) {
        frame.encoderLongPress = true;
      } else {
        frame.encoderShortPress = true;
      }
    }
  }
  lastEncoderButton_ = buttonNow ? LOW : HIGH;

  if (now - lastTouchPollMs_ >= 15) {
    lastTouchPollMs_ = now;
    const bool touchNow = touch_.touched();
    if (touchNow && !lastTouchState_ && now - lastTouchEdgeMs_ >= 220) {
      lastTouchEdgeMs_ = now;
      frame.touchPress = true;
    }
    lastTouchState_ = touchNow;
  }

  return frame;
}

void CrowPanelHardware::clearFrame(uint16_t color) { canvas_.fillSprite(color); }

void CrowPanelHardware::fillCircle(int16_t x, int16_t y, int16_t radius,
                                   uint16_t color) {
  canvas_.fillCircle(x, y, radius, color);
}

void CrowPanelHardware::drawCircle(int16_t x, int16_t y, int16_t radius,
                                   uint16_t color) {
  canvas_.drawCircle(x, y, radius, color);
}

void CrowPanelHardware::drawCenteredText(const char* text, int16_t centerY,
                                         uint8_t textSize, uint16_t color) {
  canvas_.setTextSize(textSize);
  canvas_.setTextColor(color, kColorBlack);
  const int16_t width = canvas_.textWidth(text);
  const int16_t height = canvas_.fontHeight();
  canvas_.setCursor((kDisplaySize - width) / 2, centerY - height / 2);
  canvas_.print(text);
}

void CrowPanelHardware::presentFrame() { canvas_.pushSprite(0, 0); }

void CrowPanelHardware::clearLeds() { pixels_.clear(); }

void CrowPanelHardware::setLed(int index, uint8_t r, uint8_t g, uint8_t b) {
  if (index < 0 || index >= kLedCount) return;
  pixels_.setPixelColor(index, pixels_.Color(r, g, b));
}

void CrowPanelHardware::showLeds() { pixels_.show(); }

}  // namespace mikey
