#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"
#include "LogicState.h"

namespace mikey {

class LogicActivity : public Activity {
 public:
  explicit LogicActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  void renderFrame();
  void renderInputOutputRow();
  void showLogicLeds();
  const char* ruleName() const;

  Hardware& hardware_;
  LogicState state_;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
