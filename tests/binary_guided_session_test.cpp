#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ActivityRegistry.h"
#include "BinaryActivity.h"
#include "BinaryGuidedSession.h"

using namespace mikey;

class FakeHardware final : public Hardware {
 public:
  void begin() override {}
  uint32_t nowMs() const override { return 0; }
  uint32_t randomRange(uint32_t, uint32_t) override { return 0; }
  InputFrame pollInput() override { return {}; }
  void clearFrame(uint16_t) override {}
  void fillCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCenteredText(const char* text, int16_t, uint8_t, uint16_t) override {
    if (strcmp(text, "TRY AGAIN") == 0) sawRetry = true;
  }
  void presentFrame() override { ++frames; }
  void clearLeds() override {
    for (bool& led : leds) led = false;
  }
  void setLed(int index, uint8_t r, uint8_t g, uint8_t b) override {
    assert(index >= 0 && index < 5);
    leds[index] = (r != 0 || g != 0 || b != 0);
  }
  void showLeds() override { ++ledFrames; }

  bool leds[5] = {};
  int frames = 0;
  int ledFrames = 0;
  bool sawRetry = false;
};

static void testModeGate() {
  ActivityId ids[] = {ActivityId::Jackpot, ActivityId::Reaction,
                      ActivityId::Simon, ActivityId::Dice,
                      ActivityId::Binary, ActivityId::Logic,
                      ActivityId::Debugging};
  for (ActivityId id : ids) {
    assert(isModeAvailable(id, ActivityMode::Play));
    if (id != ActivityId::Binary) {
      assert(!isModeAvailable(id, ActivityMode::Learn));
      assert(!isModeAvailable(id, ActivityMode::Challenge));
    }
  }
#if MIKEY_ENABLE_BINARY_GUIDED_PREVIEW
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Learn));
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Challenge));
#else
  assert(!isModeAvailable(ActivityId::Binary, ActivityMode::Learn));
  assert(!isModeAvailable(ActivityId::Binary, ActivityMode::Challenge));
#endif
}

static void testLearnPhasesAndWrongRetry() {
  BinaryGuidedSession session;
  session.reset(ActivityMode::Learn);
  assert(session.questionCount() == 2);
  assert(session.phase() == BinaryGuidedSession::Phase::Predict);
  assert(session.question().target == 5);
  assert(session.optionValue(0) == 5);
  assert(session.optionValue(1) == 6);
  assert(session.optionValue(2) == 0);
  assert(!session.rotate(0));
  assert(session.rotate(1));
  assert(session.selectedSlot() == 1);
  session.submit();  // Wrong B.
  assert(session.wrongAttempt());
  assert(session.questionIndex() == 0);
  assert(session.phase() == BinaryGuidedSession::Phase::Predict);
  assert(session.rotate(-1));
  assert(!session.wrongAttempt());
  session.submit();  // Correct A.
  assert(session.phase() == BinaryGuidedSession::Phase::Observe);
  assert(!session.rotate(1));
  assert(session.selectedSlot() == 0);
  session.submit();
  assert(session.phase() == BinaryGuidedSession::Phase::Explain);
  assert(strcmp(session.question().explanation, "5 = 4 + 1") == 0);
  session.submit();
  assert(session.questionIndex() == 1);
  assert(session.phase() == BinaryGuidedSession::Phase::Predict);
  assert(session.question().target == 10);
  assert(session.question().correctSlot == 1);
  assert(session.optionValue(0) == 12);
  assert(session.optionValue(1) == 10);
  session.submit();  // Wrong A.
  assert(session.wrongAttempt());
  assert(session.questionIndex() == 1);
  session.rotate(1);
  session.submit();
  assert(session.phase() == BinaryGuidedSession::Phase::Observe);
  session.submit();
  assert(session.phase() == BinaryGuidedSession::Phase::Explain);
  session.submit();
  assert(session.phase() == BinaryGuidedSession::Phase::Complete);
  assert(session.questionIndex() == session.questionCount());
  assert(!session.rotate(1));
  session.submit();  // Replay.
  assert(session.phase() == BinaryGuidedSession::Phase::Predict);
  assert(session.questionIndex() == 0);
  assert(session.selectedSlot() == 0);
}

static void testChallengeAllFour() {
  BinaryGuidedSession session;
  session.reset(ActivityMode::Challenge);
  assert(session.questionCount() == 4);
  const uint8_t targets[] = {3, 6, 9, 12};
  for (uint8_t i = 0; i < 4; ++i) {
    assert(session.questionIndex() == i);
    assert(session.question().target == targets[i]);
    assert(session.question().correctSlot == (i & 1U));
    assert(session.optionValue(session.question().correctSlot) == targets[i]);
    assert(session.optionValue(1U - session.question().correctSlot) != targets[i]);
    if ((i & 1U) != 0) session.rotate(1);
    session.submit();
    assert(session.phase() == BinaryGuidedSession::Phase::Observe);
    session.submit();
    assert(session.phase() == BinaryGuidedSession::Phase::Explain);
    session.submit();
  }
  assert(session.phase() == BinaryGuidedSession::Phase::Complete);
  session.reset(ActivityMode::Challenge);
  assert(session.questionIndex() == 0);
}

static void testAcceptedPlayRegression() {
  FakeHardware hardware;
  BinaryActivity activity(hardware);
  ActivityContext context;
  context.mode = ActivityMode::Play;
  activity.begin(context);
  assert(hardware.frames == 1);
  InputFrame increment;
  increment.encoderShortPress = true;
  increment.touchPress = true;  // One press + tap frame counts once.
  activity.handleInput(increment);
  activity.update();
  assert(hardware.leds[3]);  // 1 = 0001.
  assert(!hardware.leds[0] && !hardware.leds[1] && !hardware.leds[2]);
  assert(!hardware.leds[4]);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  for (bool led : hardware.leds) assert(!led);
}

static void testGuidedHardwareAndHome() {
  FakeHardware hardware;
  BinaryActivity activity(hardware);
  ActivityContext context;
  context.mode = ActivityMode::Learn;
  activity.begin(context);
#if MIKEY_ENABLE_BINARY_GUIDED_PREVIEW
  assert(!activity.exitRequested());
  for (bool led : hardware.leds) assert(!led);  // No answer leak.
  InputFrame turn;
  turn.encoderClockwise = true;
  activity.handleInput(turn);
  activity.update();
  InputFrame submit;
  submit.encoderShortPress = true;
  submit.touchPress = true;
  activity.handleInput(submit); // Wrong B, no extra submission.
  activity.update();
  assert(hardware.sawRetry);
  for (bool led : hardware.leds) assert(!led);
  turn = {};
  turn.encoderCounterClockwise = true;
  activity.handleInput(turn);
  activity.update();
  activity.handleInput(submit); // Correct A; enter OBSERVE once.
  activity.update();
  assert(!hardware.leds[0] && hardware.leds[1]);
  assert(!hardware.leds[2] && hardware.leds[3] && !hardware.leds[4]);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  for (bool led : hardware.leds) assert(!led);
  activity.begin(context); // Reset on re-entry.
  assert(!activity.exitRequested());
  for (bool led : hardware.leds) assert(!led);
#else
  assert(activity.exitRequested()); // Direct guided begin must fail closed.
  for (bool led : hardware.leds) assert(!led);
#endif
}

int main() {
  testModeGate();
  testLearnPhasesAndWrongRetry();
  testChallengeAllFour();
  testAcceptedPlayRegression();
  testGuidedHardwareAndHome();
  return 0;
}
