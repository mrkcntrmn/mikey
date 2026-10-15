#pragma once

#include <stdint.h>

namespace mikey {

// Four-bit value model for Binary Lab PLAY. Position 0 is the 8s bit and
// position 3 is the 1s bit so screen order matches ordinary binary notation.
class BinaryValue {
 public:
  static constexpr uint8_t kBitCount = 4;
  static constexpr uint8_t kMaxValue = 15;

  void reset() { value_ = 0; }

  void set(uint8_t value) {
    value_ = static_cast<uint8_t>(value & kMaxValue);
  }

  void increment() {
    value_ = static_cast<uint8_t>((value_ + 1) & kMaxValue);
  }

  void decrement() {
    value_ = value_ == 0 ? kMaxValue : static_cast<uint8_t>(value_ - 1);
  }

  uint8_t value() const { return value_; }

  bool bitAt(uint8_t position) const {
    if (position >= kBitCount) {
      return false;
    }
    const uint8_t shift = static_cast<uint8_t>(kBitCount - 1 - position);
    return ((value_ >> shift) & 0x01U) != 0;
  }

  static uint8_t placeValue(uint8_t position) {
    if (position >= kBitCount) {
      return 0;
    }
    return static_cast<uint8_t>(1U << (kBitCount - 1 - position));
  }

 private:
  uint8_t value_ = 0;
};

}  // namespace mikey
