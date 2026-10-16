#include "BinaryActivity.h"

#include <stdio.h>

#include "../core/ActivityRegistry.h"

namespace mikey {

BinaryActivity::BinaryActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId BinaryActivity::id() const { return ActivityId::Binary; }

const char* BinaryActivity::name() const { return "BINARY"; }

void BinaryActivity::begin(const ActivityContext& context) {
  exitRequested_ = false;
  guidedMode_ = context.mode != ActivityMode::Play;
  value_.reset();
  dirty_ = true;
  // Direct launch attempts must fail closed when preview is disabled.
  if (guidedMode_ && !isModeAvailable(ActivityId::Binary, context.mode)) {
    exitRequested_ = true;
    hardware_.clearLeds();
    hardware_.showLeds();
    return;
  }
  if (guidedMode_) {
    guided_.reset(context.mode);
    renderGuidedFrame();
  } else {
    renderFrame();  // Preserve the accepted PLAY presentation.
  }
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

  if (exitRequested_) return;
  if (guidedMode_) {
    // Priority: direction, then one submit per frame (even press + touch).
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
  if (dirty_ && !exitRequested_) {
    if (guidedMode_) renderGuidedFrame();
    else renderFrame();
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

void BinaryActivity::bitsFor(uint8_t number, char out[5]) {
  BinaryValue value;
  value.set(number);
  for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
    out[i] = value.bitAt(i) ? '1' : '0';
  }
  out[4] = 0;
}

void BinaryActivity::showGuidedLeds() {
  hardware_.clearLeds();
  const BinaryGuidedSession::Phase phase = guided_.phase();
  if (phase == BinaryGuidedSession::Phase::Observe ||
      phase == BinaryGuidedSession::Phase::Explain) {
    BinaryValue answer;
    answer.set(guided_.question().target);
    for (uint8_t i = 0; i < BinaryValue::kBitCount; ++i) {
      if (answer.bitAt(i)) hardware_.setLed(i, 0, 56, 56);
    }
  }
  // Logical LED 4 remains dark for all Binary Lab modes.
  hardware_.showLeds();
}

void BinaryActivity::renderGuidedFrame() {
  const bool lesson = guided_.mode() == ActivityMode::Learn;
  const BinaryGuidedSession::Phase phase = guided_.phase();
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText(lesson ? "BINARY LEARN" : "BINARY QUIZ",
                             29, 2, kColorYellow);

  if (phase == BinaryGuidedSession::Phase::Complete) {
    hardware_.drawCenteredText("ALL DONE!", 101, 3, kColorGreen);
    hardware_.drawCenteredText("YOU DECODED BITS", 152, 1, kColorWhite);
    hardware_.drawCenteredText("PRESS TO REPLAY", 196, 1, kColorCyan);
  } else {
    const BinaryGuidedSession::Question& q = guided_.question();
    char countText[20];
    snprintf(countText, sizeof(countText), "%u OF %u",
             static_cast<unsigned>(guided_.questionIndex() + 1),
             static_cast<unsigned>(guided_.questionCount()));
    hardware_.drawCenteredText(countText, 54, 1, kColorWhite);

    if (phase == BinaryGuidedSession::Phase::Predict) {
      char decimalText[24];
      char a[5];
      char b[5];
      bitsFor(guided_.optionValue(0), a);
      bitsFor(guided_.optionValue(1), b);
      snprintf(decimalText, sizeof(decimalText), "DECIMAL %u",
               static_cast<unsigned>(q.target));
      hardware_.drawCenteredText(decimalText, 78, 2, kColorWhite);
      hardware_.drawCenteredText("WHICH BITS MATCH?", 100, 1, kColorCyan);
      char optionText[18];
      snprintf(optionText, sizeof(optionText), "%s A: %s",
               guided_.selectedSlot() == 0 ? ">" : " ", a);
      hardware_.drawCenteredText(optionText, 128, 2,
                                 guided_.selectedSlot() == 0 ? kColorCyan : kColorWhite);
      snprintf(optionText, sizeof(optionText), "%s B: %s",
               guided_.selectedSlot() == 1 ? ">" : " ", b);
      hardware_.drawCenteredText(optionText, 156, 2,
                                 guided_.selectedSlot() == 1 ? kColorCyan : kColorWhite);
      hardware_.drawCenteredText(lesson ? "HINT: 8 4 2 1" : "PREDICT FIRST",
                                 187, 1, kColorWhite);
      hardware_.drawCenteredText(guided_.wrongAttempt()
                                     ? "TRY AGAIN"
                                     : "TURN, THEN PRESS/TAP",
                                 208, 1,
                                 guided_.wrongAttempt() ? kColorYellow : kColorCyan);
    } else if (phase == BinaryGuidedSession::Phase::Observe) {
      char bits[5];
      bitsFor(q.target, bits);
      hardware_.drawCenteredText("OBSERVE THE LEDS", 81, 1, kColorCyan);
      hardware_.drawCenteredText(bits, 120, 3, kColorGreen);
      hardware_.drawCenteredText("ON = 1  OFF = 0", 159, 1, kColorWhite);
      hardware_.drawCenteredText("PRESS FOR WHY", 206, 1, kColorCyan);
    } else {
      hardware_.drawCenteredText("EXPLAIN", 91, 2, kColorCyan);
      hardware_.drawCenteredText(q.explanation, 127, 2, kColorWhite);
      hardware_.drawCenteredText("ADD THE ON BITS", 166, 1, kColorWhite);
      hardware_.drawCenteredText("PRESS FOR NEXT", 206, 1, kColorCyan);
    }
  }
  hardware_.drawCenteredText("HOLD FOR HOME", 227, 1, kColorGray);
  hardware_.presentFrame();
  showGuidedLeds();
}

}  // namespace mikey
