#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ActivityRegistry.h"
#include "BinaryActivity.h"
#include "BinaryGuidedSession.h"
#include "LogicActivity.h"
#include "LogicGuidedSession.h"

using namespace mikey;

// Software-only CrowPanel round-screen layout check. This is a conservative
// approximation (default built-in 6x8 bitmap font), not a device screenshot.
class RoundScreenProbe final : public Hardware {
 public:
  void begin() override {}
  uint32_t nowMs() const override { return 0; }
  uint32_t randomRange(uint32_t, uint32_t) override { return 0; }
  InputFrame pollInput() override { return {}; }
  void clearFrame(uint16_t) override {}
  void fillCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCenteredText(const char* value, int16_t centerY, uint8_t size, uint16_t) override {
    const int32_t textHalfWidth = (static_cast<int32_t>(strlen(value)) * 6 * size + 1) / 2;
    const int32_t textHalfHeight = 4 * size;
    const int32_t dx = textHalfWidth + 1;
    const int32_t dy = (centerY > 120 ? centerY - 120 : 120 - centerY) +
                       textHalfHeight + 1;
    if (dx * dx + dy * dy > 120 * 120) {
      fprintf(stderr, "Outside estimated round screen: '%s' at y=%d size=%u, dx=%ld dy=%ld\n",
              value, static_cast<int>(centerY), static_cast<unsigned>(size),
              static_cast<long>(dx), static_cast<long>(dy));
      ++violations;
    }
    ++textCalls;
  }
  void presentFrame() override { ++frames; }
  void clearLeds() override {
    for (bool& led : leds) led = false;
  }
  void setLed(int index, uint8_t r, uint8_t g, uint8_t b) override {
    assert(index >= 0 && index < 5);
    leds[index] = r || g || b;
  }
  void showLeds() override { ++ledFrames; }

  unsigned violations = 0;
  unsigned frames = 0;
  unsigned textCalls = 0;
  unsigned ledFrames = 0;
  bool leds[5] = {};
};

static InputFrame turn() {
  InputFrame input;
  input.encoderClockwise = true;
  return input;
}

static InputFrame press() {
  InputFrame input;
  input.encoderShortPress = true;
  return input;
}

static void drawEveryBinaryGuidedFrame(RoundScreenProbe& hw, ActivityMode mode) {
  BinaryActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = mode;
  BinaryGuidedSession model;
  model.reset(mode);
  activity.begin(ctx);
  assert(!activity.exitRequested());
  for (uint8_t i = 0; i < model.questionCount(); ++i) {
    if (model.question().correctSlot == 1) {
      activity.handleInput(turn());
      activity.update();
      model.rotate(1);
    }
    activity.handleInput(press());
    activity.update(); // Observe.
    model.submit();
    for (bool led : hw.leds) {
      (void)led; // LED checks are in the behavioral test, not text geometry.
    }
    activity.handleInput(press());
    activity.update(); // Explain.
    model.submit();
    activity.handleInput(press());
    activity.update(); // Next prediction or completion.
    model.submit();
  }
  assert(model.phase() == BinaryGuidedSession::Phase::Complete);
  activity.handleInput(press());
  activity.update(); // Replay initial prediction.
  activity.end();
}

static void drawEveryLogicGuidedFrame(RoundScreenProbe& hw, ActivityMode mode) {
  LogicActivity activity(hw);
  ActivityContext ctx;
  ctx.mode = mode;
  LogicGuidedSession model;
  model.reset(mode);
  activity.begin(ctx);
  assert(!activity.exitRequested());
  for (uint8_t i = 0; i < model.questionCount(); ++i) {
    if (model.expectedOutput()) {
      activity.handleInput(turn());
      activity.update();
      model.rotate(1);
    }
    activity.handleInput(press());
    activity.update(); // Observe.
    model.submit();
    activity.handleInput(press());
    activity.update(); // Explain.
    model.submit();
    activity.handleInput(press());
    activity.update(); // Next prediction or completion.
    model.submit();
  }
  assert(model.phase() == LogicGuidedSession::Phase::Complete);
  activity.handleInput(press());
  activity.update(); // Replay.
  activity.end();
}

int main() {
  assert(MIKEY_ENABLE_BINARY_GUIDED_PREVIEW == 1);
  assert(MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW == 1);
  RoundScreenProbe hw;
  drawEveryBinaryGuidedFrame(hw, ActivityMode::Learn);
  drawEveryBinaryGuidedFrame(hw, ActivityMode::Challenge);
  drawEveryLogicGuidedFrame(hw, ActivityMode::Learn);
  drawEveryLogicGuidedFrame(hw, ActivityMode::Challenge);
  assert(hw.frames >= 20);
  assert(hw.textCalls >= 80);
  assert(hw.ledFrames >= hw.frames);
  assert(hw.violations == 0);
  return 0;
}
