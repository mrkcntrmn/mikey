#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"
#include "BinaryValue.h"
#include "BinaryGuidedSession.h"

namespace mikey {

// PLAY mirrors four bits to logical LEDs 0-3; logical LED 4 is intentionally unused.
class BinaryActivity : public Activity {
 public:
  explicit BinaryActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  void renderFrame();
  void renderGuidedFrame();
  void showGuidedLeds();
  static void bitsFor(uint8_t value, char out[5]);
  void renderBits();
  void showBitLeds();
  void binaryText(char out[5]) const;

  Hardware& hardware_;
  BinaryValue value_;
  BinaryGuidedSession guided_;
  bool guidedMode_ = false;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
