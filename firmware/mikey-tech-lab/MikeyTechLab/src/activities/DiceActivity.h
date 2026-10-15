#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"
#include "DiceSession.h"

namespace mikey {

enum class DiceState : uint8_t {
  Ready = 0,
  Rolling,
  Result,
};

class DiceActivity : public Activity {
 public:
  explicit DiceActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  static constexpr uint8_t kRollFrames = 9;
  static constexpr uint32_t kRollFrameMs = 70;

  void startRoll();
  void advanceRoll();
  void finishRoll();
  void renderFrame();
  void drawDieFace(uint8_t face, uint16_t color);
  void showFaceLeds(uint8_t face);
  uint16_t faceColor(uint8_t face) const;
  void faceLedColor(uint8_t face, uint8_t& r, uint8_t& g, uint8_t& b) const;

  Hardware& hardware_;
  DiceSession session_;
  DiceState state_ = DiceState::Ready;
  uint8_t currentFace_ = 1;
  uint8_t framesRemaining_ = 0;
  uint32_t nextFrameMs_ = 0;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
