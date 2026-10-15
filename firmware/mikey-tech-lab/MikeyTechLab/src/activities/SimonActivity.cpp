#include "SimonActivity.h"

#include <stdio.h>

namespace mikey {

SimonActivity::SimonActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId SimonActivity::id() const { return ActivityId::Simon; }

const char* SimonActivity::name() const { return "SIMON"; }

void SimonActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  resetSession();
  renderFrame();
  dirty_ = false;
}

void SimonActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void SimonActivity::resetSession() {
  playbackIndex_ = 0;
  selectedLight_ = 0;
  failedExpected_ = 0;
  failedActual_ = 0;
  sequence_.reset(static_cast<uint8_t>(hardware_.randomRange(
      0, static_cast<uint32_t>(SimonSequence::kLightCount))));
  state_ = SimonState::Ready;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

void SimonActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (state_ == SimonState::Input) {
    if (input.encoderClockwise) {
      selectedLight_ = SimonSequence::nextLight(selectedLight_);
      dirty_ = true;
    }
    if (input.encoderCounterClockwise) {
      selectedLight_ = SimonSequence::previousLight(selectedLight_);
      dirty_ = true;
    }
    if (input.encoderShortPress || input.touchPress) {
      submitSelected();
    }
    return;
  }

  if (!(input.encoderShortPress || input.touchPress)) {
    return;
  }

  switch (state_) {
    case SimonState::Ready:
      beginPlayback();
      break;
    case SimonState::Failure:
      beginPlayback();
      break;
    case SimonState::Complete:
      resetSession();
      break;
    case SimonState::PlaybackOn:
    case SimonState::PlaybackGap:
    case SimonState::Success:
    case SimonState::Input:
      break;
  }
}

void SimonActivity::update() {
  const uint32_t now = hardware_.nowMs();

  if (state_ == SimonState::PlaybackOn &&
      static_cast<int32_t>(now - stateStartedMs_) >=
          static_cast<int32_t>(kPlaybackOnMs)) {
    state_ = SimonState::PlaybackGap;
    stateStartedMs_ = now;
    dirty_ = true;
  } else if (state_ == SimonState::PlaybackGap &&
             static_cast<int32_t>(now - stateStartedMs_) >=
                 static_cast<int32_t>(kPlaybackGapMs)) {
    ++playbackIndex_;
    if (playbackIndex_ >= sequence_.length()) {
      enterInput();
    } else {
      state_ = SimonState::PlaybackOn;
      stateStartedMs_ = now;
      dirty_ = true;
    }
  } else if (state_ == SimonState::Success &&
             static_cast<int32_t>(now - stateStartedMs_) >=
                 static_cast<int32_t>(kSuccessHoldMs)) {
    appendStep();
    beginPlayback();
  }

  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void SimonActivity::appendStep() {
  sequence_.append(static_cast<uint8_t>(hardware_.randomRange(
      0, static_cast<uint32_t>(SimonSequence::kLightCount))));
}

void SimonActivity::beginPlayback() {
  playbackIndex_ = 0;
  selectedLight_ = 0;
  state_ = SimonState::PlaybackOn;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

void SimonActivity::enterInput() {
  state_ = SimonState::Input;
  sequence_.beginInput();
  selectedLight_ = 0;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

void SimonActivity::submitSelected() {
  const uint8_t expected = sequence_.expected();
  const uint8_t actual = selectedLight_;
  const SimonSubmitResult result = sequence_.submit(actual);

  if (result == SimonSubmitResult::Mismatch) {
    enterFailure(expected, actual);
    return;
  }

  if (result == SimonSubmitResult::RoundComplete) {
    if (sequence_.length() >= SimonSequence::kMaxLength) {
      enterComplete();
    } else {
      enterSuccess();
    }
    return;
  }

  dirty_ = true;
}

void SimonActivity::enterSuccess() {
  state_ = SimonState::Success;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

void SimonActivity::enterFailure(uint8_t expected, uint8_t actual) {
  failedExpected_ = expected;
  failedActual_ = actual;
  state_ = SimonState::Failure;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

void SimonActivity::enterComplete() {
  state_ = SimonState::Complete;
  stateStartedMs_ = hardware_.nowMs();
  dirty_ = true;
}

uint16_t SimonActivity::displayColor(uint8_t light) const {
  switch (light % SimonSequence::kLightCount) {
    case 0:
      return kColorRed;
    case 1:
      return kColorGreen;
    case 2:
      return kColorBlue;
    case 3:
      return kColorYellow;
    default:
      return kColorMagenta;
  }
}

void SimonActivity::ledColor(uint8_t light, uint8_t scale,
                             uint8_t& r, uint8_t& g, uint8_t& b) const {
  r = 0;
  g = 0;
  b = 0;
  switch (light % SimonSequence::kLightCount) {
    case 0:
      r = scale;
      break;
    case 1:
      g = scale;
      break;
    case 2:
      b = scale;
      break;
    case 3:
      r = scale;
      g = static_cast<uint8_t>(scale * 3 / 4);
      break;
    default:
      r = scale;
      b = scale;
      break;
  }
}

void SimonActivity::showSingleLed(uint8_t light, bool dim) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  ledColor(light, dim ? 36 : 96, r, g, b);
  hardware_.clearLeds();
  hardware_.setLed(light, r, g, b);
  hardware_.showLeds();
}

void SimonActivity::showAllLeds(uint8_t r, uint8_t g, uint8_t b) {
  hardware_.clearLeds();
  for (int i = 0; i < kLedCountLogical; ++i) {
    hardware_.setLed(i, r, g, b);
  }
  hardware_.showLeds();
}

void SimonActivity::drawLightRow(int activeLight, bool selection) {
  static constexpr int16_t kX[SimonSequence::kLightCount] = {40, 80, 120, 160, 200};
  static constexpr int16_t kY = 132;

  for (uint8_t i = 0; i < SimonSequence::kLightCount; ++i) {
    const bool active = activeLight >= 0 && i == static_cast<uint8_t>(activeLight);
    const int16_t radius = active ? 14 : 10;
    if (active) {
      hardware_.fillCircle(kX[i], kY, radius, displayColor(i));
      if (selection) {
        hardware_.drawCircle(kX[i], kY, radius + 4, kColorWhite);
      }
    } else {
      hardware_.drawCircle(kX[i], kY, radius, displayColor(i));
    }
  }
}

void SimonActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);

  char roundText[20];
  snprintf(roundText, sizeof(roundText), "LENGTH %u",
           static_cast<unsigned>(sequence_.length()));

  switch (state_) {
    case SimonState::Ready:
      hardware_.drawCenteredText("SIMON", 34, 3, kColorYellow);
      hardware_.drawCenteredText("LIGHTS", 68, 2, kColorYellow);
      hardware_.drawCenteredText("REMEMBER", 104, 1, kColorCyan);
      drawLightRow(-1, false);
      hardware_.drawCenteredText("TAP OR PRESS", 174, 1, kColorWhite);
      hardware_.drawCenteredText("TO START", 196, 2, kColorWhite);
      hardware_.drawCenteredText("HOLD FOR HOME", 222, 1, kColorGray);
      hardware_.clearLeds();
      hardware_.showLeds();
      break;

    case SimonState::PlaybackOn:
      hardware_.drawCenteredText("WATCH", 38, 3, kColorYellow);
      hardware_.drawCenteredText(roundText, 78, 1, kColorWhite);
      drawLightRow(sequence_.at(playbackIndex_), false);
      hardware_.drawCenteredText("REMEMBER THE ORDER", 190, 1, kColorCyan);
      showSingleLed(sequence_.at(playbackIndex_), false);
      break;

    case SimonState::PlaybackGap:
      hardware_.drawCenteredText("WATCH", 38, 3, kColorYellow);
      hardware_.drawCenteredText(roundText, 78, 1, kColorWhite);
      drawLightRow(-1, false);
      hardware_.drawCenteredText("REMEMBER THE ORDER", 190, 1, kColorCyan);
      hardware_.clearLeds();
      hardware_.showLeds();
      break;

    case SimonState::Input: {
      char stepText[20];
      snprintf(stepText, sizeof(stepText), "STEP %u OF %u",
               static_cast<unsigned>(sequence_.inputIndex() + 1),
               static_cast<unsigned>(sequence_.length()));
      hardware_.drawCenteredText("YOUR TURN", 38, 2, kColorYellow);
      hardware_.drawCenteredText(stepText, 78, 1, kColorWhite);
      drawLightRow(selectedLight_, true);
      hardware_.drawCenteredText("TURN TO CHOOSE", 176, 1, kColorCyan);
      hardware_.drawCenteredText("PRESS OR TAP", 198, 1, kColorWhite);
      hardware_.drawCenteredText("HOLD FOR HOME", 222, 1, kColorGray);
      showSingleLed(selectedLight_, true);
      break;
    }

    case SimonState::Success: {
      char successText[24];
      snprintf(successText, sizeof(successText), "%u REMEMBERED!",
               static_cast<unsigned>(sequence_.length()));
      hardware_.drawCenteredText("NICE!", 58, 3, kColorGreen);
      hardware_.drawCenteredText(successText, 110, 2, kColorWhite);
      hardware_.drawCenteredText("ADDING ONE MORE", 164, 1, kColorCyan);
      showAllLeds(0, 24, 0);
      break;
    }

    case SimonState::Failure: {
      char expectedText[28];
      char actualText[28];
      snprintf(expectedText, sizeof(expectedText), "EXPECTED LIGHT %u",
               static_cast<unsigned>(failedExpected_ + 1));
      snprintf(actualText, sizeof(actualText), "YOU PICKED LIGHT %u",
               static_cast<unsigned>(failedActual_ + 1));
      hardware_.drawCenteredText("NOT QUITE", 46, 2, kColorRed);
      hardware_.drawCenteredText(expectedText, 92, 1, kColorWhite);
      hardware_.drawCenteredText(actualText, 116, 1, kColorWhite);
      drawLightRow(failedExpected_, false);
      hardware_.drawCenteredText("TAP TO REPLAY", 190, 1, kColorCyan);
      hardware_.drawCenteredText("SAME SEQUENCE", 212, 1, kColorGray);
      showAllLeds(24, 0, 0);
      break;
    }

    case SimonState::Complete:
      hardware_.drawCenteredText("MEMORY", 46, 2, kColorYellow);
      hardware_.drawCenteredText("MASTER!", 80, 3, kColorGreen);
      hardware_.drawCenteredText("16 IN A ROW", 130, 2, kColorWhite);
      hardware_.drawCenteredText("TAP TO RESET", 184, 1, kColorCyan);
      hardware_.drawCenteredText("HOLD FOR HOME", 216, 1, kColorGray);
      showAllLeds(0, 20, 20);
      break;
  }

  hardware_.presentFrame();
}

}  // namespace mikey
