#pragma once

#include "../core/Activity.h"
#include "../core/InputEvent.h"
#include "../hardware/Hardware.h"

namespace mikey {

enum class SimonState : uint8_t {
  Ready = 0,
  PlaybackOn,
  PlaybackGap,
  Input,
  Success,
  Failure,
  Complete,
};

class SimonActivity : public Activity {
 public:
  explicit SimonActivity(Hardware& hardware);

  ActivityId id() const override;
  const char* name() const override;

  void begin(const ActivityContext& context) override;
  void handleInput(const InputFrame& input) override;
  void update() override;
  void end() override;
  bool exitRequested() const override { return exitRequested_; }

 private:
  static constexpr uint8_t kLightCount = 5;
  static constexpr uint8_t kMaxSequence = 16;
  static constexpr uint32_t kPlaybackOnMs = 500;
  static constexpr uint32_t kPlaybackGapMs = 220;
  static constexpr uint32_t kSuccessHoldMs = 900;

  void resetSession();
  void beginPlayback();
  void enterInput();
  void submitSelected();
  void enterSuccess();
  void enterFailure(uint8_t expected, uint8_t actual);
  void enterComplete();
  void appendStep();
  void renderFrame();
  void drawLightRow(int activeLight, bool selection);
  void showSingleLed(uint8_t light, bool dim);
  void showAllLeds(uint8_t r, uint8_t g, uint8_t b);
  uint16_t displayColor(uint8_t light) const;
  void ledColor(uint8_t light, uint8_t scale,
                uint8_t& r, uint8_t& g, uint8_t& b) const;

  Hardware& hardware_;
  SimonState state_ = SimonState::Ready;
  uint8_t sequence_[kMaxSequence] = {};
  uint8_t sequenceLength_ = 1;
  uint8_t playbackIndex_ = 0;
  uint8_t inputIndex_ = 0;
  uint8_t selectedLight_ = 0;
  uint8_t bestLength_ = 0;
  uint8_t failedExpected_ = 0;
  uint8_t failedActual_ = 0;
  uint32_t stateStartedMs_ = 0;
  bool exitRequested_ = false;
  bool dirty_ = true;
};

}  // namespace mikey
