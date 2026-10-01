#pragma once

#include <stdint.h>

namespace mikey {

// Hardware-independent session state: native CI can verify the learning rules.
class DiceSession {
 public:
  static constexpr uint8_t kFaceCount = 6;

  static uint8_t faceFromRandomIndex(uint8_t index) {
    return static_cast<uint8_t>((index % kFaceCount) + 1);
  }

  void reset() {
    totalRolls_ = 0;
    for (uint8_t i = 0; i < kFaceCount; ++i) {
      counts_[i] = 0;
    }
  }

  bool record(uint8_t face) {
    if (face < 1 || face > kFaceCount) {
      return false;
    }
    ++counts_[face - 1];
    ++totalRolls_;
    return true;
  }

  uint32_t totalRolls() const { return totalRolls_; }

  uint32_t countFor(uint8_t face) const {
    if (face < 1 || face > kFaceCount) {
      return 0;
    }
    return counts_[face - 1];
  }

 private:
  uint32_t counts_[kFaceCount] = {};
  uint32_t totalRolls_ = 0;
};

}  // namespace mikey
