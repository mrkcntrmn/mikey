#include "DiceActivity.h"

#include <stdio.h>

namespace mikey {

DiceActivity::DiceActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId DiceActivity::id() const { return ActivityId::Dice; }

const char* DiceActivity::name() const { return "DICE"; }

void DiceActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  session_.reset();
  state_ = DiceState::Ready;
  currentFace_ = 1;
  framesRemaining_ = 0;
  nextFrameMs_ = 0;
  dirty_ = true;
  renderFrame();
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

  if (state_ == DiceState::Rolling) {
    return;
  }

  if (input.encoderShortPress || input.touchPress) {
    startRoll();
  }
}

void DiceActivity::update() {
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
  currentFace_ = static_cast<uint8_t>(
      hardware_.randomRange(1, static_cast<uint32_t>(DiceSession::kFaceCount + 1)));

  if (framesRemaining_ > 0) {
    --framesRemaining_;
  }

  if (framesRemaining_ == 0) {
    finishRoll();
    return;
  }

  nextFrameMs_ = hardware_.nowMs() + kRollFrameMs;
  dirty_ = true;
}

void DiceActivity::finishRoll() {
  session_.record(currentFace_);
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

void DiceActivity::showFaceLeds(uint8_t face) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  faceLedColor(face, r, g, b);

  hardware_.clearLeds();
  for (int i = 0; i < kLedCountLogical; ++i) {
    hardware_.setLed(i, r, g, b);
  }
  hardware_.showLeds();
}

void DiceActivity::drawDieFace(uint8_t face, uint16_t color) {
  static constexpr int16_t kLeft = 92;
  static constexpr int16_t kCenter = 120;
  static constexpr int16_t kRight = 148;
  static constexpr int16_t kTop = 94;
  static constexpr int16_t kMiddle = 122;
  static constexpr int16_t kBottom = 150;
  static constexpr int16_t kPipRadius = 8;

  hardware_.drawCircle(kCenter, kMiddle, 62, kColorWhite);

  const bool leftTop = face >= 4;
  const bool rightTop = face >= 2;
  const bool leftMiddle = face == 6;
  const bool center = (face % 2) == 1;
  const bool rightMiddle = face == 6;
  const bool leftBottom = face >= 2;
  const bool rightBottom = face >= 4;

  if (leftTop) hardware_.fillCircle(kLeft, kTop, kPipRadius, color);
  if (rightTop) hardware_.fillCircle(kRight, kTop, kPipRadius, color);
  if (leftMiddle) hardware_.fillCircle(kLeft, kMiddle, kPipRadius, color);
  if (center) hardware_.fillCircle(kCenter, kMiddle, kPipRadius, color);
  if (rightMiddle) hardware_.fillCircle(kRight, kMiddle, kPipRadius, color);
  if (leftBottom) hardware_.fillCircle(kLeft, kBottom, kPipRadius, color);
  if (rightBottom) hardware_.fillCircle(kRight, kBottom, kPipRadius, color);
}

void DiceActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);

  switch (state_) {
    case DiceState::Ready:
      hardware_.drawCenteredText("DIGITAL", 34, 2, kColorYellow);
      hardware_.drawCenteredText("DICE", 66, 3, kColorYellow);
      hardware_.drawCenteredText("RANDOM ROLL", 102, 1, kColorCyan);
      drawDieFace(1, kColorWhite);
      hardware_.drawCenteredText("TAP OR PRESS", 194, 1, kColorWhite);
      hardware_.drawCenteredText("TO ROLL", 214, 1, kColorCyan);
      hardware_.clearLeds();
      hardware_.showLeds();
      break;

    case DiceState::Rolling:
      hardware_.drawCenteredText("ROLLING...", 42, 2, kColorYellow);
      drawDieFace(currentFace_, faceColor(currentFace_));
      hardware_.drawCenteredText("WAIT FOR IT", 202, 1, kColorGray);
      showFaceLeds(currentFace_);
      break;

    case DiceState::Result: {
      char rollText[24];
      char seenText[28];
      snprintf(rollText, sizeof(rollText), "ROLL %lu",
               static_cast<unsigned long>(session_.totalRolls()));
      snprintf(seenText, sizeof(seenText), "THIS FACE: %lu",
               static_cast<unsigned long>(session_.countFor(currentFace_)));

      hardware_.drawCenteredText(rollText, 30, 1, kColorCyan);
      drawDieFace(currentFace_, faceColor(currentFace_));
      hardware_.drawCenteredText(seenText, 190, 1, kColorWhite);
      hardware_.drawCenteredText("TAP TO ROLL AGAIN", 212, 1, kColorGray);
      showFaceLeds(currentFace_);
      break;
    }
  }

  hardware_.presentFrame();
}

}  // namespace mikey
