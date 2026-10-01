#include "LogicActivity.h"

namespace mikey {

LogicActivity::LogicActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId LogicActivity::id() const { return ActivityId::Logic; }

const char* LogicActivity::name() const { return "LOGIC"; }

void LogicActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  state_.reset();
  dirty_ = true;
  renderFrame();
  dirty_ = false;
}

void LogicActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void LogicActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (input.touchPress) {
    state_.toggleA();
    dirty_ = true;
  }

  if (input.encoderShortPress) {
    state_.toggleB();
    dirty_ = true;
  }

  if (input.encoderClockwise) {
    state_.nextRule();
    dirty_ = true;
  }

  if (input.encoderCounterClockwise) {
    state_.previousRule();
    dirty_ = true;
  }
}

void LogicActivity::update() {
  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

const char* LogicActivity::ruleName() const {
  switch (state_.rule()) {
    case LogicRule::And:
      return "AND";
    case LogicRule::Or:
      return "OR";
    case LogicRule::NotA:
      return "NOT A";
  }
  return "?";
}

void LogicActivity::renderInputOutputRow() {
  static constexpr int16_t kAX = 58;
  static constexpr int16_t kBX = 120;
  static constexpr int16_t kOutX = 182;
  static constexpr int16_t kY = 118;
  static constexpr int16_t kRadius = 17;

  if (state_.a()) {
    hardware_.fillCircle(kAX, kY, kRadius, kColorCyan);
  } else {
    hardware_.drawCircle(kAX, kY, kRadius, kColorGray);
  }

  if (state_.b()) {
    hardware_.fillCircle(kBX, kY, kRadius, kColorYellow);
  } else {
    hardware_.drawCircle(kBX, kY, kRadius, kColorGray);
  }

  if (state_.output()) {
    hardware_.fillCircle(kOutX, kY, kRadius, kColorGreen);
  } else {
    hardware_.drawCircle(kOutX, kY, kRadius, kColorRed);
  }
}

void LogicActivity::showLogicLeds() {
  hardware_.clearLeds();

  if (state_.a()) {
    hardware_.setLed(0, 0, 56, 56);
  }
  if (state_.b()) {
    hardware_.setLed(1, 56, 42, 0);
  }

  if (state_.output()) {
    hardware_.setLed(4, 0, 64, 0);
  } else {
    hardware_.setLed(4, 48, 0, 0);
  }

  hardware_.showLeds();
}

void LogicActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("LOGIC LAB", 24, 2, kColorYellow);
  hardware_.drawCenteredText(ruleName(), 58, 3, kColorWhite);

  if (state_.rule() == LogicRule::NotA) {
    hardware_.drawCenteredText("NOT USES A ONLY", 84, 1, kColorCyan);
  } else {
    hardware_.drawCenteredText("A + B INPUTS", 84, 1, kColorCyan);
  }

  renderInputOutputRow();
  hardware_.drawCenteredText("A          B        OUT", 150, 1, kColorWhite);
  hardware_.drawCenteredText("TOUCH=A  PRESS=B", 184, 1, kColorCyan);
  hardware_.drawCenteredText("TURN = CHANGE RULE", 204, 1, kColorWhite);
  hardware_.drawCenteredText("HOLD FOR HOME", 226, 1, kColorGray);
  hardware_.presentFrame();

  showLogicLeds();
}

}  // namespace mikey
