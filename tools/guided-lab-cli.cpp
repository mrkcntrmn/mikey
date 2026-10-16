// Offline terminal preview of the real Binary/Logic activity implementations.
// This tool displays text and LED *commands*, not a pixel-faithful CrowPanel.
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

#include "ActivityRegistry.h"
#include "BinaryActivity.h"
#include "LogicActivity.h"

using namespace mikey;

class TerminalHardware final : public Hardware {
 public:
  void begin() override {}
  uint32_t nowMs() const override { return 0; }
  uint32_t randomRange(uint32_t minInclusive, uint32_t) override {
    return minInclusive;
  }
  InputFrame pollInput() override { return {}; }
  void clearFrame(uint16_t) override {
    lines_.clear();
    framePending_ = false;
  }
  void fillCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCircle(int16_t, int16_t, int16_t, uint16_t) override {}
  void drawCenteredText(const char* value, int16_t centerY, uint8_t, uint16_t) override {
    lines_.push_back({centerY, value});
  }
  void presentFrame() override { framePending_ = true; }
  void clearLeds() override {
    for (char& led : leds_) led = '-';
  }
  void setLed(int index, uint8_t r, uint8_t g, uint8_t b) override {
    if (index < 0 || index >= 5) return;
    // R/Y/C/G indicates the physical cue; '-' is OFF.
    if (g && b) leds_[index] = 'C';
    else if (r && g) leds_[index] = 'Y';
    else if (g) leds_[index] = 'G';
    else if (r) leds_[index] = 'R';
    else if (b) leds_[index] = 'B';
  }
  void showLeds() override {
    if (!framePending_) return;
    std::sort(lines_.begin(), lines_.end(),
              [](const Line& a, const Line& b) { return a.y < b.y; });
    std::cout << "\n----- simulated display (top to bottom) -----\n";
    for (const Line& line : lines_) std::cout << line.text << "\n";
    std::cout << "LEDs [";
    for (unsigned i = 0; i < 5; ++i) {
      if (i) std::cout << " ";
      std::cout << leds_[i];
    }
    std::cout << "]\n";
    std::cout << "---------------------------------------------\n" << std::flush;
    framePending_ = false;
  }

 private:
  struct Line {
    int16_t y;
    std::string text;
  };
  std::vector<Line> lines_;
  char leds_[5] = {'-', '-', '-', '-', '-'};
  bool framePending_ = false;
};

static bool driveActivity(Activity& activity, ActivityMode mode) {
  ActivityContext ctx;
  ctx.mode = mode;
  activity.begin(ctx);
  if (activity.exitRequested()) {
    std::cout << "SOON: preview flag disabled.\n";
    activity.end();
    return true;
  }
  std::cout << "Controls: a/< = left, d/> = right, Enter/p = press, "
               "t = touch, h = Home, q = quit\n";

  std::string command;
  while (std::getline(std::cin, command)) {
    InputFrame input;
    if (command == "a" || command == "<") input.encoderCounterClockwise = true;
    else if (command == "d" || command == ">") input.encoderClockwise = true;
    else if (command.empty() || command == "p") input.encoderShortPress = true;
    else if (command == "t") input.touchPress = true;
    else if (command == "h") input.encoderLongPress = true;
    else if (command == "q") {
      activity.end();
      return false;
    } else {
      std::cout << "Unknown control. Try a/d, Enter, t, h, q.\n";
      continue;
    }
    activity.handleInput(input);
    if (activity.exitRequested()) {
      activity.end();
      return true;
    }
    activity.update();
  }
  activity.end();
  return false; // EOF means the preview is finished.
}

int main() {
  TerminalHardware hw;
  BinaryActivity binary(hw);
  LogicActivity logic(hw);
  std::cout << "Mikey Tech Lab — OFFLINE guided preview\n";
  std::cout << "Display text and LEDs are emulated; no GPIO or board attached.\n";

  while (true) {
    std::cout << "\n1 Binary LEARN    2 Binary CHALLENGE\n"
                 "3 Logic LEARN     4 Logic CHALLENGE\n"
                 "q Quit\n> " << std::flush;
    std::string option;
    if (!std::getline(std::cin, option) || option == "q") return 0;
    Activity* activity = nullptr;
    ActivityMode mode = ActivityMode::Learn;
    if (option == "1" || option == "2") {
      activity = &binary;
      mode = option == "1" ? ActivityMode::Learn : ActivityMode::Challenge;
    } else if (option == "3" || option == "4") {
      activity = &logic;
      mode = option == "3" ? ActivityMode::Learn : ActivityMode::Challenge;
    } else {
      std::cout << "Choose 1, 2, 3, 4, or q.\n";
      continue;
    }
    if (!isModeAvailable(activity->id(), mode)) {
      std::cout << "SOON: this build did not opt into the guided preview.\n";
      continue;
    }
    if (!driveActivity(*activity, mode)) return 0;
  }
}
