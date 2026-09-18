#include "JackpotActivity.h"

#include <stdio.h>

namespace mikey {

JackpotActivity::JackpotActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId JackpotActivity::id() const { return ActivityId::Jackpot; }

const char* JackpotActivity::name() const { return "JACKPOT"; }

void JackpotActivity::begin(const ActivityContext& /*context*/) {
  wheelPosition_ = 0;
  speedPercent_ = 100;
  running_ = true;
  lastWheelTickMs_ = hardware_.nowMs();
  exitRequested_ = false;
  dirty_ = true;
  renderFrame();
}

void JackpotActivity::update() {
  advanceWheelIfNeeded();
  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void JackpotActivity::end() {
  exitRequested_ = false;
  running_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void JackpotActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (input.encoderClockwise) {
    changeSpeed(kSpeedStepPercent);
  }
  if (input.encoderCounterClockwise) {
    changeSpeed(-kSpeedStepPercent);
  }
  if (input.encoderShortPress || input.touchPress) {
    toggleRunning();
  }
}

uint32_t JackpotActivity::wheelIntervalMs() const {
  const uint32_t interval =
      (kBaseStepMs * 100UL) / static_cast<uint32_t>(speedPercent_);
  return interval < 10UL ? 10UL : interval;
}

bool JackpotActivity::isJackpot() const {
  return wheelPosition_ == kJackpotPosition;
}

int JackpotActivity::physicalLedIndex() const {
  int index = (wheelPosition_ * kLedCountLogical) / kWheelPositions;
  if (index < 0) index = 0;
  if (index > kLedCountLogical - 1) index = kLedCountLogical - 1;
  if (kReversePhysicalLedOrder) {
    index = (kLedCountLogical - 1) - index;
  }
  return index;
}

void JackpotActivity::updatePhysicalLeds() {
  hardware_.clearLeds();

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  if (running_) {
    r = 0;
    g = 110;
    b = 160;
  } else if (isJackpot()) {
    r = 0;
    g = 180;
    b = 40;
  } else {
    r = 180;
    g = 20;
    b = 0;
  }

  hardware_.setLed(physicalLedIndex(), r, g, b);
  hardware_.showLeds();
}

void JackpotActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("JACKPOT", 10, 1, kColorYellow);

  for (int i = 0; i < kWheelPositions; ++i) {
    uint16_t color = (i == kJackpotPosition) ? kColorYellow : kColorGray;

    if (i == wheelPosition_) {
      if (running_) {
        color = kColorCyan;
      } else {
        color = isJackpot() ? kColorGreen : kColorRed;
      }
    }

    hardware_.fillCircle(kWheelX[i], kWheelY[i], 9, color);
    hardware_.drawCircle(kWheelX[i], kWheelY[i], 10, kColorWhite);
  }

  if (running_) {
    hardware_.drawCenteredText("SPINNING", 104, 2, kColorWhite);
    hardware_.drawCenteredText("TOUCH TO STOP", 134, 1, kColorCyan);
  } else if (isJackpot()) {
    hardware_.drawCenteredText("JACKPOT!", 104, 2, kColorGreen);
    hardware_.drawCenteredText("TOUCH TO SPIN", 134, 1, kColorWhite);
  } else {
    hardware_.drawCenteredText("MISS", 104, 2, kColorRed);
    hardware_.drawCenteredText("TOUCH TO SPIN", 134, 1, kColorWhite);
  }

  char speedText[24];
  snprintf(speedText, sizeof(speedText), "SPEED %d%%", speedPercent_);
  hardware_.drawCenteredText(speedText, 231, 1, kColorWhite);

  hardware_.presentFrame();
  updatePhysicalLeds();
}

void JackpotActivity::toggleRunning() {
  running_ = !running_;
  lastWheelTickMs_ = hardware_.nowMs();
  dirty_ = true;
}

void JackpotActivity::changeSpeed(int delta) {
  int next = speedPercent_ + delta;
  if (next < kMinSpeedPercent) next = kMinSpeedPercent;
  if (next > kMaxSpeedPercent) next = kMaxSpeedPercent;
  if (next == speedPercent_) return;
  speedPercent_ = next;
  lastWheelTickMs_ = hardware_.nowMs();
  dirty_ = true;
}

void JackpotActivity::advanceWheelIfNeeded() {
  if (!running_) return;

  const uint32_t now = hardware_.nowMs();
  const uint32_t interval = wheelIntervalMs();
  if (now - lastWheelTickMs_ < interval) return;

  lastWheelTickMs_ = now;
  wheelPosition_ = (wheelPosition_ + 1) % kWheelPositions;
  dirty_ = true;
}

}  // namespace mikey
