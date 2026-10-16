#include "DiceActivity.h"

#include <stdio.h>

#include "../core/ActivityRegistry.h"

namespace mikey {

DiceActivity::DiceActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId DiceActivity::id() const { return ActivityId::Dice; }

const char* DiceActivity::name() const { return "DUAL DICE"; }

void DiceActivity::begin(const ActivityContext& context) {
  exitRequested_ = false;
  guidedMode_ = context.mode != ActivityMode::Play;
  session_.reset();
  state_ = DiceState::Ready;
  leftFace_ = 1;
  rightFace_ = 1;
  framesRemaining_ = 0;
  nextFrameMs_ = 0;
  dirty_ = true;
  if (guidedMode_ && !isModeAvailable(ActivityId::Dice, context.mode)) {
    exitRequested_ = true; // Deny a direct, ungated guided-mode launch.
    hardware_.clearLeds();
    hardware_.showLeds();
    return;
  }
  if (guidedMode_) {
    guided_.reset(context.mode);
    renderGuidedFrame();
  } else {
    renderFrame();
  }
  dirty_ = false;
}

void DiceActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void DiceActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (exitRequested_) return;
  if (guidedMode_) {
    if (input.encoderClockwise) {
      dirty_ = guided_.rotate(1) || dirty_;
    } else if (input.encoderCounterClockwise) {
      dirty_ = guided_.rotate(-1) || dirty_;
    } else if (input.encoderShortPress || input.touchPress) {
      guided_.submit(); // Exactly one submit even with press + touch.
      dirty_ = true;
    }
    return;
  }

  if (state_ == DiceState::Rolling) return;
  if (input.encoderShortPress || input.touchPress) startRoll();
}

void DiceActivity::update() {
  if (exitRequested_) return;
  if (guidedMode_) {
    if (dirty_) {
      renderGuidedFrame();
      dirty_ = false;
    }
    return;
  }
  if (state_ == DiceState::Rolling &&
      static_cast<int32_t>(hardware_.nowMs() - nextFrameMs_) >= 0) {
    advanceRoll();
  }
  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void DiceActivity::startRoll() {
  state_ = DiceState::Rolling;
  framesRemaining_ = kRollFrames;
  nextFrameMs_ = hardware_.nowMs();
  dirty_ = true;
}

void DiceActivity::advanceRoll() {
  // Two separate RNG draws per animation frame, including the final pair.
  leftFace_ = DualDiceSession::faceFromRandomIndex(static_cast<uint8_t>(
      hardware_.randomRange(0, static_cast<uint32_t>(DiceSession::kFaceCount))));
  rightFace_ = DualDiceSession::faceFromRandomIndex(static_cast<uint8_t>(
      hardware_.randomRange(0, static_cast<uint32_t>(DiceSession::kFaceCount))));
  if (framesRemaining_ > 0) --framesRemaining_;
  if (framesRemaining_ == 0) {
    finishRoll();
    return;
  }
  nextFrameMs_ = hardware_.nowMs() + kRollFrameMs;
  dirty_ = true;
}

void DiceActivity::finishRoll() {
  session_.record(leftFace_, rightFace_); // One completed pair, one count.
  state_ = DiceState::Result;
  nextFrameMs_ = 0;
  dirty_ = true;
}

uint16_t DiceActivity::faceColor(uint8_t face) const {
  switch (face) {
    case 1:
      return kColorRed;
    case 2:
      return kColorGreen;
    case 3:
      return kColorBlue;
    case 4:
      return kColorYellow;
    case 5:
      return kColorMagenta;
    default:
      return kColorCyan;
  }
}

void DiceActivity::faceLedColor(uint8_t face,
                                uint8_t& r, uint8_t& g, uint8_t& b) const {
  r = 0;
  g = 0;
  b = 0;
  switch (face) {
    case 1:
      r = 56;
      break;
    case 2:
      g = 56;
      break;
    case 3:
      b = 56;
      break;
    case 4:
      r = 48;
      g = 36;
      break;
    case 5:
      r = 44;
      b = 44;
      break;
    default:
      g = 40;
      b = 40;
      break;
  }
}

void DiceActivity::showFaceLeds(uint8_t left, uint8_t right) {
  uint8_t r = 0, g = 0, b = 0;
  hardware_.clearLeds();
  faceLedColor(left, r, g, b);
  hardware_.setLed(0, r, g, b);
  hardware_.setLed(1, r, g, b);
  faceLedColor(right, r, g, b);
  hardware_.setLed(3, r, g, b);
  hardware_.setLed(4, r, g, b);
  if (left == right) hardware_.setLed(2, 0, 40, 40); // Doubles cue.
  hardware_.showLeds();
}

void DiceActivity::drawDieFace(uint8_t face, int16_t cx, uint16_t color) {
  const int16_t cy = 119;
  const int16_t delta = 17;
  const int16_t left = cx - delta;
  const int16_t right = cx + delta;
  const int16_t top = cy - delta;
  const int16_t bottom = cy + delta;
  const int16_t pipRadius = 6;
  // Centers 72 and 168, radius 40: both circles fit the 240x240 disc.
  hardware_.drawCircle(cx, cy, 40, kColorWhite);
  if (face >= 4) hardware_.fillCircle(left, top, pipRadius, color);
  if (face >= 2) hardware_.fillCircle(right, top, pipRadius, color);
  if (face == 6) hardware_.fillCircle(left, cy, pipRadius, color);
  if (face % 2 == 1) hardware_.fillCircle(cx, cy, pipRadius, color);
  if (face == 6) hardware_.fillCircle(right, cy, pipRadius, color);
  if (face >= 2) hardware_.fillCircle(left, bottom, pipRadius, color);
  if (face >= 4) hardware_.fillCircle(right, bottom, pipRadius, color);
}

void DiceActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("DUAL DICE", 36, 2, kColorYellow);
  if (state_ == DiceState::Ready) {
    hardware_.drawCenteredText("TWO FAIR DICE", 62, 1, kColorWhite);
  } else if (state_ == DiceState::Rolling) {
    hardware_.drawCenteredText("ROLLING BOTH", 62, 1, kColorCyan);
  } else {
    char rolls[24];
    snprintf(rolls, sizeof(rolls), "ROLL %lu",
             static_cast<unsigned long>(session_.totalRolls()));
    hardware_.drawCenteredText(rolls, 62, 1, kColorWhite);
  }

  drawDieFace(leftFace_, 72, faceColor(leftFace_));
  drawDieFace(rightFace_, 168, faceColor(rightFace_));

  char total[28];
  const uint8_t sum = DualDiceSession::sum(leftFace_, rightFace_);
  snprintf(total, sizeof(total), "%u + %u = %u",
           static_cast<unsigned>(leftFace_), static_cast<unsigned>(rightFace_),
           static_cast<unsigned>(sum));
  hardware_.drawCenteredText(total, 177, 2, kColorGreen);
  if (state_ == DiceState::Result) {
    char stats[32];
    snprintf(stats, sizeof(stats), "SUM %u SEEN %lu",
             static_cast<unsigned>(sum),
             static_cast<unsigned long>(session_.countForTotal(sum)));
    hardware_.drawCenteredText(stats, 201, 1, kColorCyan);
  } else {
    hardware_.drawCenteredText(state_ == DiceState::Ready
                                   ? "PRESS OR TAP TO ROLL"
                                   : "WAIT FOR BOTH DICE",
                               201, 1, kColorCyan);
  }
  hardware_.drawCenteredText("HOLD FOR HOME", 225, 1, kColorGray);
  hardware_.presentFrame();
  if (state_ == DiceState::Ready) {
    hardware_.clearLeds();
    hardware_.showLeds();
  } else {
    showFaceLeds(leftFace_, rightFace_);
  }
}

void DiceActivity::renderGuidedFrame() {
  const bool lesson = guided_.mode() == ActivityMode::Learn;
  const DiceGuidedSession::Phase phase = guided_.phase();
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText(lesson ? "DICE LEARN" : "DICE CHALLENGE",
                             36, lesson ? 2 : 1, kColorYellow);

  if (phase == DiceGuidedSession::Phase::Complete) {
    hardware_.drawCenteredText("ALL DONE!", 106, 3, kColorGreen);
    hardware_.drawCenteredText("TWO DICE EXPERT", 159, 1, kColorWhite);
    hardware_.drawCenteredText("PRESS TO REPLAY", 199, 1, kColorCyan);
  } else {
    const DiceGuidedSession::Question& q = guided_.question();
    char progress[18];
    snprintf(progress, sizeof(progress), "%u OF %u",
             static_cast<unsigned>(guided_.questionIndex() + 1),
             static_cast<unsigned>(guided_.questionCount()));
    hardware_.drawCenteredText(progress, 62, 1, kColorWhite);
    if (phase == DiceGuidedSession::Phase::Predict) {
      hardware_.drawCenteredText(q.prompt, 95, 1, kColorWhite);
      hardware_.drawCenteredText(guided_.selectedYes() ? "  NO" : "> NO",
                                 132, 2,
                                 guided_.selectedYes() ? kColorWhite : kColorCyan);
      hardware_.drawCenteredText(guided_.selectedYes() ? "> YES" : "  YES",
                                 163, 2,
                                 guided_.selectedYes() ? kColorCyan : kColorWhite);
      hardware_.drawCenteredText(guided_.retry()
                                     ? "TRY AGAIN" : "TURN THEN PRESS/TAP",
                                 198, 1,
                                 guided_.retry() ? kColorYellow : kColorCyan);
    } else if (phase == DiceGuidedSession::Phase::Observe) {
      hardware_.drawCenteredText("FIXED SAMPLE", 75, 1, kColorCyan);
      drawDieFace(q.left, 72, faceColor(q.left));
      drawDieFace(q.right, 168, faceColor(q.right));
      char sample[24];
      snprintf(sample, sizeof(sample), "%u + %u = %u",
               static_cast<unsigned>(q.left), static_cast<unsigned>(q.right),
               static_cast<unsigned>(guided_.exampleTotal()));
      hardware_.drawCenteredText(sample, 180, 2, kColorGreen);
      hardware_.drawCenteredText("PRESS FOR WHY", 205, 1, kColorCyan);
    } else {
      hardware_.drawCenteredText("EXPLAIN", 96, 2, kColorCyan);
      hardware_.drawCenteredText(q.explanation, 144, 1, kColorWhite);
      hardware_.drawCenteredText("PRESS FOR NEXT", 200, 1, kColorCyan);
    }
  }
  hardware_.drawCenteredText("HOLD FOR HOME", 225, 1, kColorGray);
  hardware_.presentFrame();
  if (phase == DiceGuidedSession::Phase::Observe ||
      phase == DiceGuidedSession::Phase::Explain) {
    const DiceGuidedSession::Question& q = guided_.question();
    showFaceLeds(q.left, q.right);
  } else {
    hardware_.clearLeds(); // No sample-answer LED leak on PREDICT/retry.
    hardware_.showLeds();
  }
}

}  // namespace mikey
