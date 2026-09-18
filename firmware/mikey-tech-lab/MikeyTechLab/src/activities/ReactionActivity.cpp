#include "ReactionActivity.h"

#include <stdio.h>

namespace mikey {

ReactionActivity::ReactionActivity(Hardware& hardware) : hardware_(hardware) {}

ActivityId ReactionActivity::id() const { return ActivityId::Reaction; }

const char* ReactionActivity::name() const { return "REACTION"; }

void ReactionActivity::begin(const ActivityContext& /*context*/) {
  exitRequested_ = false;
  hasBest_ = false;
  bestReactionMs_ = 0;
  lastReactionMs_ = 0;
  cueScheduledMs_ = 0;
  cueShownMs_ = 0;
  enterReady();
  renderFrame();
  dirty_ = false;
}

void ReactionActivity::end() {
  exitRequested_ = false;
  hardware_.clearLeds();
  hardware_.showLeds();
}

void ReactionActivity::armTouchGrace(uint32_t graceMs) {
  ignoreTouchUntilMs_ = hardware_.nowMs() + graceMs;
}

bool ReactionActivity::touchAllowed() const {
  return static_cast<int32_t>(hardware_.nowMs() - ignoreTouchUntilMs_) >= 0;
}

void ReactionActivity::handleInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    exitRequested_ = true;
    return;
  }

  if (!input.touchPress || !touchAllowed()) {
    return;
  }

  switch (state_) {
    case ReactionState::Ready:
      enterWaiting();
      break;
    case ReactionState::Waiting:
      enterFalseStart();
      break;
    case ReactionState::Go:
      enterResult(hardware_.nowMs() - cueShownMs_);
      break;
    case ReactionState::Result:
    case ReactionState::FalseStart:
      enterWaiting();
      break;
  }
}

void ReactionActivity::update() {
  if (state_ == ReactionState::Waiting &&
      static_cast<int32_t>(hardware_.nowMs() - cueScheduledMs_) >= 0) {
    enterGo();
  }

  if (dirty_) {
    renderFrame();
    dirty_ = false;
  }
}

void ReactionActivity::enterReady() {
  state_ = ReactionState::Ready;
  cueScheduledMs_ = 0;
  cueShownMs_ = 0;
  armTouchGrace(kTouchGraceMs);
  dirty_ = true;
}

void ReactionActivity::enterWaiting() {
  state_ = ReactionState::Waiting;
  const uint32_t waitMs =
      hardware_.randomRange(kMinWaitMs, kMaxWaitMsExclusive);
  cueScheduledMs_ = hardware_.nowMs() + waitMs;
  cueShownMs_ = 0;
  // Swallow the tap that started/retried the round so it cannot also count
  // as a false start on the next edge.
  armTouchGrace(kTouchGraceMs);
  dirty_ = true;
}

void ReactionActivity::enterGo() {
  state_ = ReactionState::Go;
  cueShownMs_ = hardware_.nowMs();
  // Brief grace only: reject contact already held through WAIT, but do not
  // delay a real reaction press.
  armTouchGrace(kGoTouchGraceMs);
  dirty_ = true;
}

void ReactionActivity::enterResult(uint32_t reactionMs) {
  state_ = ReactionState::Result;
  lastReactionMs_ = reactionMs;
  if (!hasBest_ || lastReactionMs_ < bestReactionMs_) {
    bestReactionMs_ = lastReactionMs_;
    hasBest_ = true;
  }
  armTouchGrace(kTouchGraceMs);
  dirty_ = true;
}

void ReactionActivity::enterFalseStart() {
  state_ = ReactionState::FalseStart;
  cueScheduledMs_ = 0;
  cueShownMs_ = 0;
  armTouchGrace(kTouchGraceMs);
  dirty_ = true;
}

void ReactionActivity::setAllLeds(uint8_t r, uint8_t g, uint8_t b) {
  hardware_.clearLeds();
  for (int i = 0; i < kLedCountLogical; ++i) {
    hardware_.setLed(i, r, g, b);
  }
  hardware_.showLeds();
}

void ReactionActivity::renderFrame() {
  hardware_.clearFrame(kColorBlack);

  switch (state_) {
    case ReactionState::Ready:
      hardware_.drawCenteredText("REACTION", 36, 2, kColorYellow);
      hardware_.drawCenteredText("RACER", 68, 2, kColorYellow);
      hardware_.drawCenteredText("READY?", 108, 2, kColorCyan);
      hardware_.drawCenteredText("TAP", 148, 3, kColorWhite);
      hardware_.drawCenteredText("TO START", 182, 1, kColorWhite);
      hardware_.drawCenteredText("HOLD FOR HOME", 216, 1, kColorGray);
      hardware_.clearLeds();
      hardware_.showLeds();
      break;

    case ReactionState::Waiting:
      hardware_.drawCenteredText("REACTION", 40, 2, kColorYellow);
      hardware_.drawCenteredText("RACER", 72, 2, kColorYellow);
      hardware_.drawCenteredText("GET READY", 116, 2, kColorWhite);
      hardware_.drawCenteredText("WAIT FOR", 152, 1, kColorGray);
      hardware_.drawCenteredText("GO", 186, 3, kColorWhite);
      hardware_.clearLeds();
      hardware_.showLeds();
      break;

    case ReactionState::Go:
      hardware_.drawCenteredText("GO!", 110, 4, kColorGreen);
      setAllLeds(0, kLedGoG, 0);
      break;

    case ReactionState::Result: {
      char resultText[24];
      snprintf(resultText, sizeof(resultText), "%lu ms",
               static_cast<unsigned long>(lastReactionMs_));
      char bestText[28];
      snprintf(bestText, sizeof(bestText), "BEST %lu ms",
               static_cast<unsigned long>(bestReactionMs_));

      hardware_.drawCenteredText("REACTION", 34, 2, kColorYellow);
      hardware_.drawCenteredText(resultText, 88, 3, kColorWhite);
      hardware_.drawCenteredText(bestText, 128, 1, kColorCyan);
      hardware_.drawCenteredText("TAP", 168, 2, kColorWhite);
      hardware_.drawCenteredText("TO RACE AGAIN", 200, 1, kColorGray);
      setAllLeds(0, kLedResultG, kLedResultB);
      break;
    }

    case ReactionState::FalseStart:
      hardware_.drawCenteredText("TOO SOON!", 48, 2, kColorRed);
      hardware_.drawCenteredText("WAIT FOR", 96, 1, kColorWhite);
      hardware_.drawCenteredText("GO", 128, 3, kColorWhite);
      hardware_.drawCenteredText("TAP", 172, 2, kColorCyan);
      hardware_.drawCenteredText("TO RETRY", 204, 1, kColorGray);
      setAllLeds(kLedFalseR, 0, 0);
      break;
  }

  hardware_.presentFrame();
}

}  // namespace mikey
