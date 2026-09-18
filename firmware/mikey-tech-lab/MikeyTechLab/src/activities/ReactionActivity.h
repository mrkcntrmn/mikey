#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"

namespace mikey {

enum class ReactionState : uint8_t {
  Ready = 0,
  Waiting,
  Go,
  Result,
  FalseStart,
};

class ReactionActivity : public Activity {
 public:
  explicit ReactionActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  static constexpr uint32_t kMinWaitMs = 1500;
  static constexpr uint32_t kMaxWaitMsExclusive = 4001;
  // Ignore touch briefly after transitions so the launch/start tap cannot
  // cascade into Waiting → FalseStart, and so a held finger through WAIT
  // cannot look like a dead GO screen.
  static constexpr uint32_t kTouchGraceMs = 350;
  static constexpr uint32_t kGoTouchGraceMs = 40;

  // Keep all-five LED patterns dim: full-bright quints can brown out USB power.
  static constexpr uint8_t kLedGoG = 48;
  static constexpr uint8_t kLedFalseR = 48;
  static constexpr uint8_t kLedResultG = 24;
  static constexpr uint8_t kLedResultB = 40;

  void enterReady();
  void enterWaiting();
  void enterGo();
  void enterResult(uint32_t reactionMs);
  void enterFalseStart();
  void armTouchGrace(uint32_t graceMs);
  bool touchAllowed() const;
  void setAllLeds(uint8_t r, uint8_t g, uint8_t b);
  void renderFrame();

  Hardware& hardware_;
  ReactionState state_ = ReactionState::Ready;
  uint32_t cueScheduledMs_ = 0;
  uint32_t cueShownMs_ = 0;
  uint32_t lastReactionMs_ = 0;
  uint32_t bestReactionMs_ = 0;
  uint32_t ignoreTouchUntilMs_ = 0;
  bool hasBest_ = false;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
