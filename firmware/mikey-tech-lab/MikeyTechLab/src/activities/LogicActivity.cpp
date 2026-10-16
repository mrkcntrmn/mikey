#include "LogicActivity.h"

#include <stdio.h>

#include "../core/ActivityRegistry.h"

namespace mikey {

LogicActivity::LogicActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId LogicActivity::id() const { return ActivityId::Logic; }

const char* LogicActivity::name() const { return "LOGIC"; }

void LogicActivity::begin(const ActivityContext& context) {
  exitRequested_ = false;
  guidedMode_ = context.mode != ActivityMode::Play;
  state_.reset();
  dirty_ = true;
  if (guidedMode_ && !isModeAvailable(ActivityId::Logic, context.mode)) {
    // The app menu gate is not sufficient; reject direct guided launches, too.
    exitRequested_ = true;
    hardware_.clearLeds();
    hardware_.showLeds();
    return;
  }

  if (guidedMode_) {
    guided_.reset(context.mode);
    renderGuidedFrame();
  } else {
    renderFrame();  // Keep the accepted PLAY behavior intact.
  }
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

  if (exitRequested_) return;
  if (guidedMode_) {
    // Rotation selects output 0/1; press OR touch submits once per input frame.
    if (input.encoderClockwise) {
      dirty_ = guided_.rotate(1) || dirty_;
    } else if (input.encoderCounterClockwise) {
      dirty_ = guided_.rotate(-1) || dirty_;
    } else if (input.encoderShortPress || input.touchPress) {
      guided_.submit();
      dirty_ = true;
    }
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
  if (dirty_ && !exitRequested_) {
    if (guidedMode_) renderGuidedFrame();
    else renderFrame();
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

const char* LogicActivity::guidedRuleName(LogicRule rule) {
  switch (rule) {
    case LogicRule::And: return "AND";
    case LogicRule::Or: return "OR";
    case LogicRule::NotA: return "NOT A";
  }
  return "?";
}

void LogicActivity::showGuidedLeds() {
  hardware_.clearLeds();
  if (guided_.phase() == LogicGuidedSession::Phase::Observe ||
      guided_.phase() == LogicGuidedSession::Phase::Explain) {
    const LogicGuidedSession::Question& q = guided_.question();
    if (q.a) hardware_.setLed(0, 0, 56, 56);
    if (q.b) hardware_.setLed(1, 56, 42, 0);
    if (guided_.expectedOutput()) {
      hardware_.setLed(4, 0, 64, 0);
    } else {
      hardware_.setLed(4, 48, 0, 0);
    }
  }
  // LEDs remain off during prediction, retry and completion.
  hardware_.showLeds();
}

void LogicActivity::renderGuidedFrame() {
  const bool lesson = guided_.mode() == ActivityMode::Learn;
  const LogicGuidedSession::Phase phase = guided_.phase();
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText(lesson ? "LOGIC LEARN" : "LOGIC CHALLENGE",
                             31, lesson ? 2 : 1, kColorYellow);

  if (phase == LogicGuidedSession::Phase::Complete) {
    hardware_.drawCenteredText("ALL DONE!", 105, 3, kColorGreen);
    hardware_.drawCenteredText("YOU FOUND THE RULES", 157, 1, kColorWhite);
    hardware_.drawCenteredText("PRESS TO REPLAY", 201, 1, kColorCyan);
  } else {
    const LogicGuidedSession::Question& q = guided_.question();
    char line[26];
    snprintf(line, sizeof(line), "%u OF %u",
             static_cast<unsigned>(guided_.questionIndex() + 1),
             static_cast<unsigned>(guided_.questionCount()));
    hardware_.drawCenteredText(line, 55, 1, kColorWhite);

    snprintf(line, sizeof(line), "RULE: %s", guidedRuleName(q.rule));
    hardware_.drawCenteredText(line, 77, 2, kColorWhite);
    snprintf(line, sizeof(line), "A=%u   B=%u",
             static_cast<unsigned>(q.a), static_cast<unsigned>(q.b));
    hardware_.drawCenteredText(line, 105, 2, kColorCyan);

    if (phase == LogicGuidedSession::Phase::Predict) {
      hardware_.drawCenteredText("PREDICT THE OUTPUT", 125, 1, kColorWhite);
      hardware_.drawCenteredText(guided_.selectedOutput() ? "  0: OFF" : "> 0: OFF",
                                 150, 2,
                                 guided_.selectedOutput() ? kColorWhite : kColorCyan);
      hardware_.drawCenteredText(guided_.selectedOutput() ? "> 1: ON" : "  1: ON",
                                 175, 2,
                                 guided_.selectedOutput() ? kColorCyan : kColorWhite);
      hardware_.drawCenteredText(guided_.wrongAttempt() ? "TRY AGAIN" :
                                 (lesson ? "TRY THE RULE" : "MAKE A PREDICTION"),
                                 199, 1, kColorYellow);
    } else if (phase == LogicGuidedSession::Phase::Observe) {
      hardware_.drawCenteredText("OBSERVE THE LEDS", 133, 1, kColorWhite);
      hardware_.drawCenteredText(guided_.expectedOutput() ?
                                 "OUTPUT = 1" : "OUTPUT = 0",
                                 163, 2, kColorGreen);
      hardware_.drawCenteredText("PRESS FOR WHY", 203, 1, kColorCyan);
    } else {
      hardware_.drawCenteredText("EXPLAIN", 133, 2, kColorYellow);
      hardware_.drawCenteredText(q.explanation, 163, 1, kColorWhite);
      hardware_.drawCenteredText("PRESS FOR NEXT", 203, 1, kColorCyan);
    }
  }

  hardware_.drawCenteredText("HOLD FOR HOME", 227, 1, kColorGray);
  hardware_.presentFrame();
  showGuidedLeds();
}

}  // namespace mikey
