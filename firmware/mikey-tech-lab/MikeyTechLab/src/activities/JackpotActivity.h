#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"

namespace mikey {

class JackpotActivity : public Activity {
 public:
  explicit JackpotActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  static constexpr int kWheelPositions = 12;
  static constexpr int kJackpotPosition = 0;
  static constexpr int kMinSpeedPercent = 25;
  static constexpr int kMaxSpeedPercent = 1000;
  static constexpr int kSpeedStepPercent = 25;
  static constexpr uint32_t kBaseStepMs = 150;
  static constexpr bool kReversePhysicalLedOrder = true;

  static constexpr int16_t kWheelX[kWheelPositions] = {
      120, 163, 194, 206, 194, 163, 120, 77, 46, 34, 46, 77};
  static constexpr int16_t kWheelY[kWheelPositions] = {
      34, 46, 77, 120, 163, 194, 206, 194, 163, 120, 77, 46};

  uint32_t wheelIntervalMs() const;
  bool isJackpot() const;
  int physicalLedIndex() const;
  void updatePhysicalLeds();
  void renderFrame();
  void toggleRunning();
  void changeSpeed(int delta);
  void advanceWheelIfNeeded();

  Hardware& hardware_;
  int wheelPosition_ = 0;
  int speedPercent_ = 100;
  bool running_ = true;
  uint32_t lastWheelTickMs_ = 0;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
