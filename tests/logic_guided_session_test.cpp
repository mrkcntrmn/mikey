#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ActivityRegistry.h"
#include "LogicActivity.h"
#include "LogicGuidedSession.h"

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
  void drawCenteredText(const char* text, int16_t y, uint8_t size, uint16_t) override {
    assert(size >= 1 && size <= 3);
    assert(y >= 0 && y < 240);
    if (strcmp(text, "TRY AGAIN") == 0) retrySeen = true;
    if (strcmp(text, "OUTPUT = 0") == 0) outputZeroSeen = true;
    if (strcmp(text, "OUTPUT = 1") == 0) outputOneSeen = true;
  }
  void presentFrame() override { ++frames; }
  void clearLeds() override {
    for (bool& led : leds) led = false;
  }
  void setLed(int index, uint8_t r, uint8_t g, uint8_t b) override {
    assert(index >= 0 && index < 5);
    leds[index] = (r || g || b);
  }
  void showLeds() override { ++ledFrames; }
  bool leds[5] = {};
  bool retrySeen = false;
  bool outputZeroSeen = false;
  bool outputOneSeen = false;
  int frames = 0;
  int ledFrames = 0;
};

#if MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW
static InputFrame pressFrame() {
  InputFrame f;
  f.encoderShortPress = true;
  f.touchPress = true; // Must be one submit, not two.
  return f;
}
#endif

static void testModeAvailabilityIndependence() {
  ActivityId ids[] = {ActivityId::Jackpot, ActivityId::Reaction, ActivityId::Simon,
                      ActivityId::Dice, ActivityId::Binary, ActivityId::Logic,
                      ActivityId::Debugging};
  for (ActivityId id : ids) {
    assert(isModeAvailable(id, ActivityMode::Play));
    if (id != ActivityId::Binary && id != ActivityId::Logic) {
      assert(!isModeAvailable(id, ActivityMode::Learn));
      assert(!isModeAvailable(id, ActivityMode::Challenge));
    }
  }
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_BINARY_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Logic, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Challenge) ==
         static_cast<bool>(MIKEY_ENABLE_BINARY_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Logic, ActivityMode::Challenge) ==
         static_cast<bool>(MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW));
}

static bool expected(LogicRule r, bool a, bool b) {
  if (r == LogicRule::And) return a && b;
  if (r == LogicRule::Or) return a || b;
  return !a; // Ignore B for NOT A.
}

static void testAllQuestionsAndTruthTables() {
  const ActivityMode modes[] = {ActivityMode::Learn, ActivityMode::Challenge};
  for (ActivityMode mode : modes) {
    LogicGuidedSession s;
    s.reset(mode);
    assert(s.questionCount() == (mode == ActivityMode::Learn ? 3 : 6));
    for (uint8_t i = 0; i < s.questionCount(); ++i) {
      assert(s.questionIndex() == i);
      const LogicGuidedSession::Question& q = s.question();
      assert(s.expectedOutput() == expected(q.rule, q.a, q.b));
      assert(q.explanation != nullptr);
      assert(strlen(q.explanation) > 0);
      assert(s.phase() == LogicGuidedSession::Phase::Predict);
      assert(!s.wrongAttempt());

      // For each question test the wrong answer, then the correct one.
      if (s.selectedOutput() == s.expectedOutput()) {
        assert(s.rotate(1));
      }
      assert(s.selectedOutput() != s.expectedOutput());
      s.submit();
      assert(s.wrongAttempt());
      assert(s.questionIndex() == i);
      assert(s.phase() == LogicGuidedSession::Phase::Predict);
      assert(s.rotate(-1)); // Either direction toggles the selection.
      assert(!s.wrongAttempt());
      assert(s.selectedOutput() == s.expectedOutput());
      s.submit();
      assert(s.phase() == LogicGuidedSession::Phase::Observe);
      assert(!s.rotate(1)); // Input must not modify previous prediction.
      s.submit();
      assert(s.phase() == LogicGuidedSession::Phase::Explain);
      s.submit();
      assert(s.questionIndex() == i + 1);
    }
    assert(s.phase() == LogicGuidedSession::Phase::Complete);
    assert(!s.rotate(1));
    s.submit();
    assert(s.phase() == LogicGuidedSession::Phase::Predict);
    assert(s.questionIndex() == 0);
    assert(!s.selectedOutput());
    assert(!s.wrongAttempt());
  }
}

static void testKnownRuleExamples() {
  LogicGuidedSession s;
  s.reset(ActivityMode::Learn);
  assert(s.question().rule == LogicRule::And);
  assert(s.question().a && !s.question().b && !s.expectedOutput());
  s.submit();  // Predicted 0 for AND(1,0).
  s.submit();
  s.submit();
  assert(s.question().rule == LogicRule::Or);
  assert(s.question().a && !s.question().b && s.expectedOutput());
  s.rotate(1);
  s.submit();
  s.submit();
  s.submit();
  assert(s.question().rule == LogicRule::NotA);
  assert(!s.question().a && s.question().b && s.expectedOutput());
  s.rotate(1);
  s.submit();
  s.submit();
  s.submit();
  assert(s.phase() == LogicGuidedSession::Phase::Complete);
}

static void testPlayRegression() {
  FakeHardware hw;
  LogicActivity activity(hw);
  ActivityContext context;
  context.mode = ActivityMode::Play;
  activity.begin(context);
  assert(hw.frames == 1);
  assert(hw.leds[4]);  // An OFF output is red in accepted PLAY.
  InputFrame touch;
  touch.touchPress = true;
  activity.handleInput(touch); // A = 1, B = 0, AND => 0.
  activity.update();
  assert(hw.leds[0] && !hw.leds[1] && hw.leds[4]);
  InputFrame rotary;
  rotary.encoderClockwise = true;
  activity.handleInput(rotary); // OR => 1.
  activity.update();
  assert(hw.leds[0] && !hw.leds[1] && hw.leds[4]);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  for (bool led : hw.leds) assert(!led);
}

static void testGuidedRenderingAndNoAnswerLeak() {
  FakeHardware hw;
  LogicActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = ActivityMode::Learn;
  activity.begin(ctx);
#if MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW
  assert(!activity.exitRequested());
  assert(hw.frames == 1);
  for (bool led : hw.leds) assert(!led); // Predict => all outputs hidden.
  InputFrame rotate;
  rotate.encoderClockwise = true;
  activity.handleInput(rotate); // Select 1 incorrectly for AND(1,0).
  activity.update();
  activity.handleInput(pressFrame()); // Press and tap same frame => one wrong attempt.
  activity.update();
  assert(hw.retrySeen);
  for (bool led : hw.leds) assert(!led);
  rotate = {};
  rotate.encoderCounterClockwise = true;
  activity.handleInput(rotate);
  activity.update();
  activity.handleInput(pressFrame()); // Correct 0 => Observe only.
  activity.update();
  assert(hw.outputZeroSeen);
  assert(hw.leds[0] && !hw.leds[1] && !hw.leds[2] && !hw.leds[3] && hw.leds[4]);
  activity.handleInput(pressFrame()); // Explain.
  activity.update();
  activity.handleInput(pressFrame()); // Next prediction, clear all LEDs.
  activity.update();
  for (bool led : hw.leds) assert(!led);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  home.touchPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  for (bool led : hw.leds) assert(!led);
  activity.begin(ctx); // Starts from initial question/blank LEDs.
  assert(!activity.exitRequested());
  for (bool led : hw.leds) assert(!led);
#else
  assert(activity.exitRequested()); // Fail closed to direct bypass.
  for (bool led : hw.leds) assert(!led);
#endif
}

int main() {
  testModeAvailabilityIndependence();
  testAllQuestionsAndTruthTables();
  testKnownRuleExamples();
  testPlayRegression();
  testGuidedRenderingAndNoAnswerLeak();
  return 0;
}
