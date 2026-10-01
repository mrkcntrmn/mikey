#include "DebugActivity.h"

#include <stdio.h>

namespace mikey {

namespace {

constexpr DebugActivity::CaseText kCases[DebugSession::kCaseCount] = {
    {"BINARY BUG", "EXPECTED 0101", "OBSERVED 0111",
     "2 BIT STUCK ON", "8 BIT STUCK ON"},
    {"LOGIC BUG", "A1 B0 AND => 0", "OBSERVED OUT 1",
     "INPUT A WRONG", "AND RULE WRONG"},
    {"SIMON BUG", "EXPECTED 2-4-1", "OBSERVED 2-3-1",
     "STEP 2 WRONG", "STEP 3 WRONG"},
    {"REACTION BUG", "EXPECTED > 0 MS", "OBSERVED 0 MS",
     "LED TOO BRIGHT", "TIMER NOT UPDATED"},
    {"DICE BUG", "EXPECTED COUNT +1", "OBSERVED COUNT +2",
     "COUNTED TWICE", "WRONG COLOR"},
};

}  // namespace

DebugActivity::DebugActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId DebugActivity::id() const { return ActivityId::Debugging; }

const char* DebugActivity::name() const { return "DEBUG"; }

const DebugActivity::CaseText& DebugActivity::caseText(uint8_t index) {
  return kCases[index % DebugSession::kCaseCount];
}

void DebugActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  session_.reset();
  state_ = DebugViewState::Inspect;
  dirty_ = true;
  renderFrame();
  dirty_ = false;
}

void DebugActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void DebugActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (state_ == DebugViewState::Inspect) {
    if (input.encoderClockwise) {
      session_.nextOption();
      dirty_ = true;
    }
    if (input.encoderCounterClockwise) {
      session_.previousOption();
      dirty_ = true;
    }
    if (input.encoderShortPress || input.touchPress) {
      submitSelection();
    }
    return;
  }

  if (!(input.encoderShortPress || input.touchPress)) {
    return;
  }

  switch (state_) {
    case DebugViewState::Correct:
    case DebugViewState::Incorrect:
      state_ = DebugViewState::Inspect;
      dirty_ = true;
      break;
    case DebugViewState::Complete:
      session_.reset();
      state_ = DebugViewState::Inspect;
      dirty_ = true;
      break;
    case DebugViewState::Inspect:
      break;
  }
}

void DebugActivity::update() {
  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void DebugActivity::submitSelection() {
  const DebugSubmitResult result = session_.submit();
  if (result == DebugSubmitResult::Incorrect) {
    state_ = DebugViewState::Incorrect;
  } else if (result == DebugSubmitResult::Complete) {
    state_ = DebugViewState::Complete;
  } else {
    state_ = DebugViewState::Correct;
  }
  dirty_ = true;
}

void DebugActivity::showSelectionLeds() {
  hardware_.clearLeds();
  if (session_.selectedOption() == 0) {
    hardware_.setLed(0, 0, 48, 48);
  } else {
    hardware_.setLed(1, 0, 48, 48);
  }
  hardware_.showLeds();
}

void DebugActivity::showAllLeds(uint8_t r, uint8_t g, uint8_t b) {
  hardware_.clearLeds();
  for (int i = 0; i < kLedCountLogical; ++i) {
    hardware_.setLed(i, r, g, b);
  }
  hardware_.showLeds();
}

void DebugActivity::renderInspect() {
  const CaseText& item = caseText(session_.caseIndex());

  char bugText[16];
  char optionA[32];
  char optionB[32];
  snprintf(bugText, sizeof(bugText), "BUG %u OF %u",
           static_cast<unsigned>(session_.caseIndex() + 1),
           static_cast<unsigned>(DebugSession::kCaseCount));
  snprintf(optionA, sizeof(optionA), "%c A %s",
           session_.selectedOption() == 0 ? '>' : ' ', item.optionA);
  snprintf(optionB, sizeof(optionB), "%c B %s",
           session_.selectedOption() == 1 ? '>' : ' ', item.optionB);

  hardware_.drawCenteredText("DEBUG DETECTIVE", 18, 2, kColorYellow);
  hardware_.drawCenteredText(bugText, 44, 1, kColorGray);
  hardware_.drawCenteredText(item.title, 66, 2, kColorCyan);
  hardware_.drawCenteredText(item.expected, 94, 1, kColorWhite);
  hardware_.drawCenteredText(item.observed, 114, 1, kColorRed);
  hardware_.drawCenteredText(optionA, 150, 1, kColorWhite);
  hardware_.drawCenteredText(optionB, 170, 1, kColorWhite);
  hardware_.drawCenteredText("TURN=CHOOSE  PRESS=CHECK", 202, 1, kColorCyan);
  hardware_.drawCenteredText("HOLD FOR HOME", 224, 1, kColorGray);
  showSelectionLeds();
}

void DebugActivity::renderResult(bool correct) {
  if (correct) {
    char solvedText[20];
    snprintf(solvedText, sizeof(solvedText), "%u OF %u SOLVED",
             static_cast<unsigned>(session_.solvedCount()),
             static_cast<unsigned>(DebugSession::kCaseCount));
    hardware_.drawCenteredText("BUG FOUND!", 72, 3, kColorGreen);
    hardware_.drawCenteredText(solvedText, 118, 2, kColorWhite);
    hardware_.drawCenteredText("COMPARE EXPECTED", 158, 1, kColorCyan);
    hardware_.drawCenteredText("WITH OBSERVED", 178, 1, kColorCyan);
    hardware_.drawCenteredText("PRESS FOR NEXT", 212, 1, kColorWhite);
    showAllLeds(0, 32, 0);
  } else {
    hardware_.drawCenteredText("NOT YET", 76, 3, kColorRed);
    hardware_.drawCenteredText("COMPARE AGAIN", 122, 2, kColorWhite);
    hardware_.drawCenteredText("WHAT CHANGED?", 158, 1, kColorCyan);
    hardware_.drawCenteredText("PRESS TO RETRY", 210, 1, kColorWhite);
    showAllLeds(32, 0, 0);
  }
}

void DebugActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);

  switch (state_) {
    case DebugViewState::Inspect:
      renderInspect();
      break;
    case DebugViewState::Correct:
      renderResult(true);
      break;
    case DebugViewState::Incorrect:
      renderResult(false);
      break;
    case DebugViewState::Complete:
      hardware_.drawCenteredText("DEBUG", 62, 3, kColorYellow);
      hardware_.drawCenteredText("DETECTIVE", 96, 3, kColorYellow);
      hardware_.drawCenteredText("ALL 5 BUGS FOUND", 142, 1, kColorGreen);
      hardware_.drawCenteredText("PRESS TO RESET", 198, 1, kColorCyan);
      hardware_.drawCenteredText("HOLD FOR HOME", 220, 1, kColorGray);
      showAllLeds(0, 36, 18);
      break;
  }

  hardware_.presentFrame();
}

}  // namespace mikey
