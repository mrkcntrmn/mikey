#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"
#include "DebugSession.h"

namespace mikey {

enum class DebugViewState : uint8_t {
  Inspect = 0,
  Correct,
  Incorrect,
  Complete,
};

class DebugActivity : public Activity {
 public:
  explicit DebugActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  struct CaseText {
    const char* title;
    const char* expected;
    const char* observed;
    const char* optionA;
    const char* optionB;
  };

  static const CaseText& caseText(uint8_t index);

  void submitSelection();
  void renderFrame();
  void renderInspect();
  void renderResult(bool correct);
  void showSelectionLeds();
  void showAllLeds(uint8_t r, uint8_t g, uint8_t b);

  Hardware& hardware_;
  DebugSession session_;
  DebugViewState state_ = DebugViewState::Inspect;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
