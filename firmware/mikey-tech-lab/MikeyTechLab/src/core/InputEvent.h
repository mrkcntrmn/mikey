#pragma once

#include <stdint.h>

namespace mikey {

enum class InputEventType : uint8_t {
  None = 0,
  EncoderClockwise,
  EncoderCounterClockwise,
  EncoderShortPress,
  EncoderLongPress,
  TouchPress,
};

struct InputFrame {
  bool encoderClockwise = false;
  bool encoderCounterClockwise = false;
  bool encoderShortPress = false;
  bool encoderLongPress = false;
  bool touchPress = false;

  bool any() const {
    return encoderClockwise || encoderCounterClockwise || encoderShortPress ||
           encoderLongPress || touchPress;
  }
};

}  // namespace mikey
