#include "BinaryActivity.h"

#include <stdio.h>

namespace mikey {

BinaryActivity::BinaryActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId BinaryActivity::id() const { return ActivityId::Binary; }

const char* BinaryActivity::name() const { return "BINARY"; }

void BinaryActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  value_.reset();
  dirty_ = true;
  renderFrame();
  dirty_ = false;
}

void BinaryActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void BinaryActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (input.encoderClockwise) {
    value_.increment();
    dirty_ = true;
  }

  if (input.encoderCounterClockwise) {
    value_.decrement();
    dirty_ = true;
  }

  if (input.encoderShortPress || input.touchPress) {
    value_.increment();
    dirty_ = true;
  }
}

void BinaryActivity::update() {
  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void BinaryActivity::binaryText(char out[5]) const {
  for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
    out[i] = value_.bitAt(i) ? '1' : '0';
  }
  out[4] = '\0';
}

void BinaryActivity::showBitLeds() {
  hardware_.clearLeds();
  for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
    if (value_.bitAt(i)) {
      hardware_.setLed(i, 0, 56, 56);
    }
  }
  hardware_.showLeds();
}

void BinaryActivity::renderBits() {
  static constexpr int16_t kX[BinaryValue::kBitCount] = {54, 98, 142, 186};
  static constexpr int16_t kY = 126;
  static constexpr int16_t kRadius = 14;

  for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
    if (value_.bitAt(i)) {
      hardware_.fillCircle(kX[i], kY, kRadius, kColorCyan);
    } else {
      hardware_.drawCircle(kX[i], kY, kRadius, kColorGray);
    }
  }
}

void BinaryActivity::renderFrame() {
  char decimalText[20];
  char bits[5];
  binaryText(bits);

  snprintf(decimalText, sizeof(decimalText), "DECIMAL %u",
           static_cast<unsigned>(value_.value()));

  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("BINARY LAB", 24, 2, kColorYellow);
  hardware_.drawCenteredText(decimalText, 56, 2, kColorWhite);
  hardware_.drawCenteredText(bits, 86, 3, kColorCyan);
  renderBits();
  hardware_.drawCenteredText("8      4      2      1", 158, 1, kColorWhite);
  hardware_.drawCenteredText("TURN TO EXPLORE 0-15", 190, 1, kColorCyan);
  hardware_.drawCenteredText("PRESS/TAP = +1", 208, 1, kColorWhite);
  hardware_.drawCenteredText("HOLD FOR HOME", 226, 1, kColorGray);
  hardware_.presentFrame();

  showBitLeds();
}

}  // namespace mikey
