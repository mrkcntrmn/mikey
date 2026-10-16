#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ActivityRegistry.h"
#include "DiceActivity.h"
#include "DiceGuidedSession.h"
#include "DualDiceSession.h"

using namespace mikey;

class FakeHardware final : public Hardware {
 public:
  int randomCalls = 0;
  int randomMode = 0;
  uint32_t now = 0;
  unsigned frames = 0;
  bool leds[5] = {};
  uint32_t colors[5] = {};
  bool retrySeen = false;
  bool sampleSeen = false;
  bool sumSevenSeen = false;
  bool sumEightSeen = false;
  bool rollRecorded = false;

  void begin() override {}
  uint32_t nowMs() const override { return now; }
  uint32_t randomRange(uint32_t min, uint32_t max) override {
    assert(min == 0 && max == 6);
    const int position = randomCalls++ % 2;
    return randomMode == 0 ? (position == 0 ? 0 : 5) : 3;
  }
  InputFrame pollInput() override { return {}; }
  void clearFrame(uint16_t) override {}
  void fillCircle(int16_t x, int16_t y, int16_t radius, uint16_t) override {
    assert(x >= 0 && x < 240 && y >= 0 && y < 240);
    assert(radius >= 1 && radius <= 40);
  }
  void drawCircle(int16_t x, int16_t y, int16_t radius, uint16_t) override {
    assert(x >= 0 && x < 240 && y >= 0 && y < 240);
    assert(radius == 40);
  }
  void drawCenteredText(const char* t, int16_t y, uint8_t, uint16_t) override {
    assert(y >= 0 && y < 240);
    if (strcmp(t, "TRY AGAIN") == 0) retrySeen = true;
    if (strcmp(t, "FIXED SAMPLE") == 0) sampleSeen = true;
    if (strcmp(t, "1 + 6 = 7") == 0) sumSevenSeen = true;
    if (strcmp(t, "4 + 4 = 8") == 0) sumEightSeen = true;
    if (strcmp(t, "SUM 7 SEEN 1") == 0) rollRecorded = true;
  }
  void presentFrame() override { ++frames; }
  void clearLeds() override {
    for (int i = 0; i < 5; ++i) {
      leds[i] = false;
      colors[i] = 0;
    }
  }
  void setLed(int i, uint8_t r, uint8_t g, uint8_t b) override {
    assert(i >= 0 && i < 5);
    leds[i] = r || g || b;
    colors[i] = (static_cast<uint32_t>(r) << 16)
              | (static_cast<uint32_t>(g) << 8) | b;
  }
  void showLeds() override {}
};

static InputFrame press() {
  InputFrame f;
  f.encoderShortPress = true;
  f.touchPress = true;
  return f;
}
#if MIKEY_ENABLE_DICE_GUIDED_PREVIEW
static InputFrame rotate() {
  InputFrame f;
  f.encoderClockwise = true;
  return f;
}
#endif
static void checkOff(const FakeHardware& hw) {
  for (bool led : hw.leds) assert(!led);
}

static void testDualDiceModel() {
  DualDiceSession s;
  assert(s.totalRolls() == 0);
  assert(s.doublesCount() == 0);
  assert(DualDiceSession::sum(1, 1) == 2);
  assert(DualDiceSession::sum(6, 6) == 12);
  assert(DualDiceSession::sum(0, 1) == 0);
  assert(DualDiceSession::sum(6, 7) == 0);
  assert(!s.record(0, 1) && !s.record(7, 6));
  for (uint8_t total = 0; total <= 14; ++total) {
    const uint8_t ways = DualDiceSession::waysForTotal(total);
    const uint8_t expected = total >= 2 && total <= 12 ?
        (total <= 7 ? total - 1 : 13 - total) : 0;
    assert(ways == expected);
  }
  assert(DualDiceSession::waysForTotal(7) == 6);
  assert(DualDiceSession::waysForTotal(2) == 1);

  for (uint8_t left = 1; left <= 6; ++left) {
    for (uint8_t right = 1; right <= 6; ++right) {
      assert(s.record(left, right));
    }
  }
  assert(s.totalRolls() == 36);
  assert(s.doublesCount() == 6);
  for (uint8_t total = 2; total <= 12; ++total) {
    assert(s.countForTotal(total) == DualDiceSession::waysForTotal(total));
  }
  for (uint8_t face = 1; face <= 6; ++face) {
    assert(s.countForLeft(face) == 6);
    assert(s.countForRight(face) == 6);
  }
  assert(s.countForTotal(1) == 0 && s.countForTotal(13) == 0);
  assert(s.countForLeft(0) == 0 && s.countForRight(7) == 0);
  s.reset();
  assert(s.totalRolls() == 0 && s.doublesCount() == 0);
  for (uint8_t i = 2; i <= 12; ++i) assert(s.countForTotal(i) == 0);
}

static void testGuidedModel() {
  const ActivityMode modes[] = {ActivityMode::Learn, ActivityMode::Challenge};
  for (ActivityMode mode : modes) {
    DiceGuidedSession s;
    s.reset(mode);
    assert(s.questionCount() == (mode == ActivityMode::Learn ? 3 : 4));
    for (uint8_t i = 0; i < s.questionCount(); ++i) {
      const DiceGuidedSession::Question& q = s.question();
      assert(DualDiceSession::validFace(q.left));
      assert(DualDiceSession::validFace(q.right));
      assert(DualDiceSession::sum(q.left, q.right) == s.exampleTotal());
      assert(strlen(q.prompt) > 2 && strlen(q.explanation) > 2);
      assert(s.phase() == DiceGuidedSession::Phase::Predict);
      assert(!s.retry());
      // Deliberately pick the wrong answer first.
      if (s.selectedYes() == q.correctYes) s.rotate(1);
      assert(s.selectedYes() != q.correctYes);
      s.submit();
      assert(s.retry());
      assert(s.phase() == DiceGuidedSession::Phase::Predict);
      assert(s.questionIndex() == i);
      s.rotate(-1);
      assert(!s.retry() && s.selectedYes() == q.correctYes);
      s.submit();
      assert(s.phase() == DiceGuidedSession::Phase::Observe);
      assert(!s.rotate(1));
      s.submit();
      assert(s.phase() == DiceGuidedSession::Phase::Explain);
      s.submit();
      assert(s.questionIndex() == i + 1);
    }
    assert(s.phase() == DiceGuidedSession::Phase::Complete);
    s.submit();
    assert(s.phase() == DiceGuidedSession::Phase::Predict);
    assert(s.questionIndex() == 0);
  }
}

static void testDefaultOffAndIndependentPreviewFlags() {
  assert(isModeAvailable(ActivityId::Dice, ActivityMode::Play));
  assert(isModeAvailable(ActivityId::Dice, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_DICE_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Dice, ActivityMode::Challenge) ==
         static_cast<bool>(MIKEY_ENABLE_DICE_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Simon, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_SIMON_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Binary, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_BINARY_GUIDED_PREVIEW));
  assert(isModeAvailable(ActivityId::Logic, ActivityMode::Learn) ==
         static_cast<bool>(MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW));
}

static void advanceNineFrames(FakeHardware& hw, DiceActivity& activity) {
  for (int frame = 0; frame < 9; ++frame) {
    hw.now += frame == 0 ? 0 : 70;
    activity.update();
  }
}

static void testRealDualPlayPath() {
  FakeHardware hw;
  DiceActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = ActivityMode::Play;
  activity.begin(ctx);
  assert(hw.frames == 1 && hw.randomCalls == 0);
  checkOff(hw);
  activity.handleInput(press()); // A second press while rolling is ignored.
  activity.handleInput(press());
  advanceNineFrames(hw, activity);
  assert(hw.randomCalls == 18); // Two independent draws per each of nine frames.
  assert(hw.sumSevenSeen && hw.rollRecorded);
  assert(!hw.leds[2]); // 1 + 6 = 7: not doubles.
  assert(hw.leds[0] && hw.leds[1] && hw.leds[3] && hw.leds[4]);
  assert(hw.colors[0] == hw.colors[1] && hw.colors[3] == hw.colors[4]);
  assert(hw.colors[0] != hw.colors[3]); // Two distinct die cues.
  hw.randomMode = 1;
  activity.handleInput(press());
  advanceNineFrames(hw, activity);
  assert(hw.randomCalls == 36);
  assert(hw.sumEightSeen && hw.leds[2]); // 4 + 4 = 8, doubles cue.
  assert(hw.colors[0] == hw.colors[3]); // Matching dice match LED colors.
  assert(hw.colors[2] != hw.colors[0]); // Doubles indicator is distinct.
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  home.touchPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  checkOff(hw);
  ctx.mode = ActivityMode::Play;
  activity.begin(ctx);
  checkOff(hw);
  assert(hw.randomCalls == 36); // No phantom rolls on re-entry.
}

static void testRealGuidedMode() {
  FakeHardware hw;
  DiceActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = ActivityMode::Learn;
  activity.begin(ctx);
#if MIKEY_ENABLE_DICE_GUIDED_PREVIEW
  assert(!activity.exitRequested());
  checkOff(hw); // A prediction may not reveal a fixed-example answer.
  assert(!hw.sampleSeen);
  assert(hw.randomCalls == 0);
  activity.handleInput(press()); // Wrong NO; first answer YES.
  activity.update();
  assert(hw.retrySeen);
  checkOff(hw);
  assert(!hw.sampleSeen);
  activity.handleInput(rotate());
  activity.update();
  activity.handleInput(press()); // YES => Observe.
  activity.update();
  assert(hw.sampleSeen);
  assert(hw.leds[2]); // 4+4 is doubles.
  activity.handleInput(press()); // Explain, not skip to next.
  activity.update();
  activity.handleInput(press()); // Next question.
  activity.update();
  checkOff(hw);
  InputFrame home;
  home.encoderLongPress = true;
  home.encoderShortPress = true;
  activity.handleInput(home);
  assert(activity.exitRequested());
  activity.end();
  checkOff(hw);
  activity.begin(ctx);
  checkOff(hw);
#else
  assert(activity.exitRequested());
  checkOff(hw);
  assert(hw.randomCalls == 0);
#endif
}

int main() {
  testDualDiceModel();
  testGuidedModel();
  testDefaultOffAndIndependentPreviewFlags();
  testRealDualPlayPath();
  testRealGuidedMode();
  return 0;
}
