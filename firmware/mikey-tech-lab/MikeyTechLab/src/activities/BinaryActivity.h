#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"
#include "BinaryValue.h"

namespace mikey {

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
  void renderBits();
  void showBitLeds();
  void binaryText(char out[5]) const;

  Hardware& hardware_;
  BinaryValue value_;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
