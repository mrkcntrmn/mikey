#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ActivityRegistry.h"
#include "SimonActivity.h"
#include "SimonGuidedSession.h"

using namespace mikey;

class FakeHardware final : public Hardware {
 public:
  uint32_t now = 0;
  int randomCalls = 0;
  unsigned frames = 0;
  bool retrySeen = false;
  bool answerSeen = false;
  bool leds[5] = {};

  void begin() override {}
  uint32_t nowMs() const override { return now; }
  uint32_t randomRange(uint32_t minInclusive, uint32_t) override {
    ++randomCalls;
    return minInclusive;
  }
  InputFrame pollInput() override { return {}; }
  void clearFrame(uint16_t) override {}
  void fillCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCenteredText(const char* t, int16_t, uint8_t, uint16_t) override {
    if (strcmp(t, "TRY AGAIN") == 0) retrySeen = true;
    if (strncmp(t, "NEXT IS LIGHT", 13) == 0) answerSeen = true;
  }
  void presentFrame() override { ++frames; }
  void clearLeds() override { for (bool& led : leds) led = false; }
  void setLed(int i, uint8_t r, uint8_t g, uint8_t b) override {
    assert(i >= 0 && i < 5);
    leds[i] = r != 0 || g != 0 || b != 0;
  }
  void showLeds() override {}
};

static InputFrame press() {
  InputFrame input;
  input.encoderShortPress = true;
  input.touchPress = true; // One frame is exactly one submit.
  return input;
}
#if MIKEY_ENABLE_SIMON_GUIDED_PREVIEW
static InputFrame turn(bool clockwise = true) {
  InputFrame input;
  input.encoderClockwise = clockwise;
  input.encoderCounterClockwise = !clockwise;
  return input;
}
#endif
static void expectAllOff(const FakeHardware& hw) {
  for (bool led : hw.leds) assert(!led);
}

static void testGate() {
  assert(isModeAvailable(ActivityId::Simon, ActivityMode::Play));
  assert(isModeAvailable(ActivityId::Simon, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_SIMON_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Simon, ActivityMode::Challenge) ==
         static_cast<bool>(MIKEY_ENABLE_SIMON_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_BINARY_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Logic, ActivityMode::Challenge) ==
         static_cast<bool>(MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW));
  const ActivityId others[] = {
      ActivityId::Jackpot, ActivityId::Reaction,
      ActivityId::Dice, ActivityId::Debugging,
  };
  for (ActivityId other : others) {
    assert(isModeAvailable(other, ActivityMode::Play));
    assert(!isModeAvailable(other, ActivityMode::Learn));
    assert(!isModeAvailable(other, ActivityMode::Challenge));
  }
}

static void testSequenceModel() {
  const ActivityMode modes[] = {ActivityMode::Learn, ActivityMode::Challenge};
  const uint8_t learnAnswers[] = {2, 3};
  const uint8_t challengeAnswers[] = {4, 1, 3};
  for (ActivityMode mode : modes) {
    SimonGuidedSession s;
    s.reset(mode);
    assert(s.phase() == SimonGuidedSession::Phase::Predict);
    assert(s.questionCount() == (mode == ActivityMode::Learn ? 2 : 3));
    for (uint8_t i = 0; i < s.questionCount(); ++i) {
      assert(s.questionIndex() == i);
      assert(s.length() >= 2 && s.length() <= 4);
      assert(s.prefixLength() + 1 == s.length());
      assert(s.prefixAt(s.prefixLength()) == 0);
      assert(s.expectedLight() == (mode == ActivityMode::Learn
          ? learnAnswers[i] : challengeAnswers[i]));
      assert(strlen(s.explanation()) > 3);
      assert(!s.rotate(0));
      assert(s.rotate(-1)); // Left wraps selection 0 -> 4.
      assert(s.selectedLight() == 4);
      assert(s.rotate(1)); // Right wraps 4 -> 0.
      assert(s.selectedLight() == 0);
      // Deliberately incorrect light, preserving the whole question.
      if (s.selectedLight() == s.expectedLight()) s.rotate(1);
      s.submit();
      assert(s.wrongAttempt());
      assert(s.questionIndex() == i);
      assert(s.phase() == SimonGuidedSession::Phase::Predict);
      // Rotate clockwise to the answer (five symbols, bounded walk).
      for (uint8_t k = 0; k < 5 && s.selectedLight() != s.expectedLight(); ++k)
        assert(s.rotate(1));
      assert(s.selectedLight() == s.expectedLight());
      assert(!s.wrongAttempt());
      s.submit();
      assert(s.phase() == SimonGuidedSession::Phase::Observe);
      assert(!s.rotate(1));
      s.submit();
      assert(s.phase() == SimonGuidedSession::Phase::Explain);
      s.submit();
      assert(s.questionIndex() == i + 1);
    }
    assert(s.phase() == SimonGuidedSession::Phase::Complete);
    assert(!s.rotate(1));
    s.submit(); // Replay.
    assert(s.phase() == SimonGuidedSession::Phase::Predict);
    assert(s.questionIndex() == 0);
    assert(s.selectedLight() == 0);
  }
}

static void testPlayStillUsesOriginalSequenceAndTiming() {
  FakeHardware hw;
  SimonActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = ActivityMode::Play;
  activity.begin(ctx);
  assert(hw.frames == 1);
  assert(hw.randomCalls == 1); // PLAY retains original RNG path.
  expectAllOff(hw);
  activity.handleInput(press()); // Start playback of light 0.
  activity.update();
  assert(hw.leds[0]);
  for (int i = 1; i < 5; ++i) assert(!hw.leds[i]);
  hw.now = 500;
  activity.update();
  expectAllOff(hw);
  hw.now = 720;
  activity.update(); // Start input.
  activity.handleInput(press()); // 0 selected initially, completes round.
  activity.update();
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  expectAllOff(hw);
}

static void testGuidedIntegration() {
  FakeHardware hw;
  SimonActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = ActivityMode::Learn;
  activity.begin(ctx);
#if MIKEY_ENABLE_SIMON_GUIDED_PREVIEW
  assert(!activity.exitRequested());
  assert(hw.randomCalls == 0); // Deterministic lesson independent from RNG.
  expectAllOff(hw);
  activity.handleInput(press()); // Wrong initial light 0 (expected light 2).
  activity.update();
  assert(hw.retrySeen);
  assert(!hw.answerSeen); // No answer text before correct guess.
  expectAllOff(hw);
  activity.handleInput(turn());
  activity.update();
  activity.handleInput(turn());
  activity.update();
  activity.handleInput(press()); // Correct light 2, enter Observe once.
  activity.update();
  assert(hw.answerSeen);
  assert(hw.leds[2]);
  for (int i = 0; i < 5; ++i) if (i != 2) assert(!hw.leds[i]);
  activity.handleInput(press()); // Explain once.
  activity.update();
  assert(hw.leds[2]);
  activity.handleInput(press()); // Next question: clear answer.
  activity.update();
  expectAllOff(hw);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  home.touchPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  expectAllOff(hw);
  activity.begin(ctx);
  assert(!activity.exitRequested());
  expectAllOff(hw);
  assert(hw.randomCalls == 0);
#else
  assert(activity.exitRequested()); // Direct guided begin fails closed.
  expectAllOff(hw);
  assert(hw.randomCalls == 0);
#endif
}

int main() {
  testGate();
  testSequenceModel();
  testPlayStillUsesOriginalSequenceAndTiming();
  testGuidedIntegration();
  return 0;
}
