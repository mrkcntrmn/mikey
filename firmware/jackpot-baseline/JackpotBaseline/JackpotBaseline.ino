#define LGFX_USE_V1

#include <Arduino.h>
#include <Wire.h>
#include <LovyanGFX.hpp>
#include <Adafruit_NeoPixel.h>

// -----------------------------------------------------------------------------
// Elecrow CrowPanel 1.28-inch HMI ESP32 Rotary Display hardware profile
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Game tuning
// -----------------------------------------------------------------------------

static constexpr int kWheelPositions = 12;
static constexpr int kJackpotPosition = 0;
static constexpr int kMinSpeedPercent = 25;
static constexpr int kMaxSpeedPercent = 1000;
static constexpr int kSpeedStepPercent = 25;
static constexpr uint32_t kBaseStepMs = 150;  // Wheel interval at 100%.
static constexpr bool kReversePhysicalLedOrder = true;

static constexpr uint16_t kBlack = 0x0000;
static constexpr uint16_t kWhite = 0xFFFF;
static constexpr uint16_t kGray = 0x4208;
static constexpr uint16_t kYellow = 0xFFE0;
static constexpr uint16_t kCyan = 0x07FF;
static constexpr uint16_t kGreen = 0x07E0;
static constexpr uint16_t kRed = 0xF800;

// Clockwise positions beginning at 12 o'clock.
static constexpr int16_t kWheelX[kWheelPositions] = {
    120, 163, 194, 206, 194, 163, 120, 77, 46, 34, 46, 77};
static constexpr int16_t kWheelY[kWheelPositions] = {
    34, 46, 77, 120, 163, 194, 206, 194, 163, 120, 77, 46};

// -----------------------------------------------------------------------------
// Display driver
// -----------------------------------------------------------------------------

class LGFX : public lgfx::LGFX_Device {
 private:
  lgfx::Panel_GC9A01 panel_;
  lgfx::Bus_SPI bus_;

 public:
  LGFX() {
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
};

LGFX display;
LGFX_Sprite canvas(&display);
Adafruit_NeoPixel pixels(kLedCount, kLedPin, NEO_GRB + NEO_KHZ800);

// -----------------------------------------------------------------------------
// Minimal CST816D touch reader
// -----------------------------------------------------------------------------

class TouchReader {
 public:
  void begin() {
    Wire1.begin(kTouchSdaPin, kTouchSclPin);
    Wire1.setClock(400000);

    pinMode(kTouchIntPin, OUTPUT);
    digitalWrite(kTouchIntPin, HIGH);
    delay(1);
    digitalWrite(kTouchIntPin, LOW);
    delay(1);

    pinMode(kTouchResetPin, OUTPUT);
    digitalWrite(kTouchResetPin, LOW);
    delay(10);
    digitalWrite(kTouchResetPin, HIGH);
    delay(300);

    writeRegister(0xFE, 0xFF);
    pinMode(kTouchIntPin, INPUT_PULLUP);
  }

  bool touched(uint16_t *x = nullptr, uint16_t *y = nullptr) {
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

 private:
  static constexpr uint8_t kAddress = 0x15;

  bool writeRegister(uint8_t reg, uint8_t value) {
    Wire1.beginTransmission(kAddress);
    Wire1.write(reg);
    Wire1.write(value);
    return Wire1.endTransmission() == 0;
  }

  bool readRegister(uint8_t reg, uint8_t *value) {
    Wire1.beginTransmission(kAddress);
    Wire1.write(reg);
    if (Wire1.endTransmission(false) != 0) return false;
    if (Wire1.requestFrom(kAddress, static_cast<uint8_t>(1)) != 1) return false;
    *value = Wire1.read();
    return true;
  }

  bool readRegisters(uint8_t reg, uint8_t *buffer, size_t length) {
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
};

TouchReader touch;

// -----------------------------------------------------------------------------
// Game state
// -----------------------------------------------------------------------------

int wheelPosition = 0;
int speedPercent = 100;
bool running = true;

int lastEncoderA = HIGH;
bool lastEncoderButton = HIGH;
bool lastTouchState = false;

uint32_t lastWheelTick = 0;
uint32_t lastButtonToggle = 0;
uint32_t lastTouchPoll = 0;
uint32_t lastTouchToggle = 0;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

uint32_t wheelIntervalMs() {
  uint32_t interval = (kBaseStepMs * 100UL) / static_cast<uint32_t>(speedPercent);
  return max<uint32_t>(interval, 10UL);
}

bool isJackpot() {
  return wheelPosition == kJackpotPosition;
}

void drawCentered(const char *text, int16_t centerY, uint8_t textSize, uint16_t color) {
  canvas.setTextSize(textSize);
  canvas.setTextColor(color, kBlack);
  int16_t width = canvas.textWidth(text);
  int16_t height = canvas.fontHeight();
  canvas.setCursor((240 - width) / 2, centerY - height / 2);
  canvas.print(text);
}

int physicalLedIndex() {
  int index = (wheelPosition * kLedCount) / kWheelPositions;
  index = constrain(index, 0, kLedCount - 1);
  if (kReversePhysicalLedOrder) {
    index = (kLedCount - 1) - index;
  }
  return index;
}

void updatePhysicalLeds() {
  pixels.clear();

  uint32_t color;
  if (running) {
    color = pixels.Color(0, 110, 160);
  } else if (isJackpot()) {
    color = pixels.Color(0, 180, 40);
  } else {
    color = pixels.Color(180, 20, 0);
  }

  pixels.setPixelColor(physicalLedIndex(), color);
  pixels.show();
}

void renderFrame() {
  canvas.fillSprite(kBlack);

  drawCentered("JACKPOT", 10, 1, kYellow);

  for (int i = 0; i < kWheelPositions; ++i) {
    uint16_t color = (i == kJackpotPosition) ? kYellow : kGray;

    if (i == wheelPosition) {
      if (running) {
        color = kCyan;
      } else {
        color = isJackpot() ? kGreen : kRed;
      }
    }

    canvas.fillCircle(kWheelX[i], kWheelY[i], 9, color);
    canvas.drawCircle(kWheelX[i], kWheelY[i], 10, kWhite);
  }

  if (running) {
    drawCentered("SPINNING", 104, 2, kWhite);
    drawCentered("TOUCH TO STOP", 134, 1, kCyan);
  } else if (isJackpot()) {
    drawCentered("JACKPOT!", 104, 2, kGreen);
    drawCentered("TOUCH TO SPIN", 134, 1, kWhite);
  } else {
    drawCentered("MISS", 104, 2, kRed);
    drawCentered("TOUCH TO SPIN", 134, 1, kWhite);
  }

  char speedText[24];
  snprintf(speedText, sizeof(speedText), "SPEED %d%%", speedPercent);
  drawCentered(speedText, 231, 1, kWhite);

  canvas.pushSprite(0, 0);
  updatePhysicalLeds();
}

void toggleRunning() {
  running = !running;
  lastWheelTick = millis();
  renderFrame();
}

void changeSpeed(int delta) {
  int next = constrain(speedPercent + delta, kMinSpeedPercent, kMaxSpeedPercent);
  if (next == speedPercent) return;
  speedPercent = next;
  lastWheelTick = millis();
  renderFrame();
}

void pollEncoder() {
  int encoderA = digitalRead(kEncoderAPin);

  if (encoderA != lastEncoderA && encoderA == HIGH) {
    int encoderB = digitalRead(kEncoderBPin);
    if (encoderB != encoderA) {
      changeSpeed(kSpeedStepPercent);
    } else {
      changeSpeed(-kSpeedStepPercent);
    }
  }
  lastEncoderA = encoderA;

  bool buttonNow = digitalRead(kEncoderSwitchPin);
  uint32_t now = millis();
  if (lastEncoderButton == HIGH && buttonNow == LOW && now - lastButtonToggle >= 180) {
    lastButtonToggle = now;
    toggleRunning();
  }
  lastEncoderButton = buttonNow;
}

void pollTouch() {
  uint32_t now = millis();
  if (now - lastTouchPoll < 15) return;
  lastTouchPoll = now;

  bool touchNow = touch.touched();
  if (touchNow && !lastTouchState && now - lastTouchToggle >= 220) {
    lastTouchToggle = now;
    toggleRunning();
  }
  lastTouchState = touchNow;
}

void advanceWheelIfNeeded() {
  if (!running) return;

  uint32_t now = millis();
  uint32_t interval = wheelIntervalMs();
  if (now - lastWheelTick < interval) return;

  lastWheelTick = now;
  wheelPosition = (wheelPosition + 1) % kWheelPositions;  // Clockwise on screen.
  renderFrame();
}

// -----------------------------------------------------------------------------
// Arduino entry points
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(50);

  // Match the power-up sequence used by Elecrow's board example.
  pinMode(kPowerLightPin, OUTPUT);
  digitalWrite(kPowerLightPin, LOW);
  pinMode(kPowerEnablePin1, OUTPUT);
  digitalWrite(kPowerEnablePin1, HIGH);
  pinMode(kPowerEnablePin2, OUTPUT);
  digitalWrite(kPowerEnablePin2, HIGH);

  touch.begin();

  display.init();
  display.setRotation(0);

  pinMode(kBacklightPin, OUTPUT);
  digitalWrite(kBacklightPin, HIGH);

  canvas.setColorDepth(16);
  canvas.createSprite(240, 240);

  pixels.begin();
  pixels.setBrightness(40);
  pixels.clear();
  pixels.show();

  pinMode(kEncoderAPin, INPUT_PULLUP);
  pinMode(kEncoderBPin, INPUT_PULLUP);
  pinMode(kEncoderSwitchPin, INPUT_PULLUP);
  lastEncoderA = digitalRead(kEncoderAPin);
  lastEncoderButton = digitalRead(kEncoderSwitchPin);

  wheelPosition = 0;
  speedPercent = 100;
  running = true;
  lastWheelTick = millis();

  renderFrame();
  Serial.println("Mikey Tech Lab Jackpot baseline ready.");
}

void loop() {
  pollEncoder();
  pollTouch();
  advanceWheelIfNeeded();
  delay(1);
}
