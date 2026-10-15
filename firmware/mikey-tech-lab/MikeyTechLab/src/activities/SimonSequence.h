#pragma once

#include <stdint.h>

namespace mikey {

enum class SimonSubmitResult : uint8_t {
  AcceptedContinue = 0,
  RoundComplete,
  Mismatch,
};

class SimonSequence {
 public:
  static constexpr uint8_t kLightCount = 5;
  static constexpr uint8_t kMaxLength = 16;

  void reset(uint8_t firstLight) {
    length_ = 1;
    inputIndex_ = 0;
    sequence_[0] = normalize(firstLight);
  }

  bool append(uint8_t light) {
    if (length_ >= kMaxLength) {
      return false;
    }
    sequence_[length_] = normalize(light);
    ++length_;
    return true;
  }

  void beginInput() { inputIndex_ = 0; }

  SimonSubmitResult submit(uint8_t light) {
    if (inputIndex_ >= length_ || normalize(light) != expected()) {
      return SimonSubmitResult::Mismatch;
    }

    ++inputIndex_;
    if (inputIndex_ >= length_) {
      return SimonSubmitResult::RoundComplete;
    }
    return SimonSubmitResult::AcceptedContinue;
  }

  uint8_t length() const { return length_; }
  uint8_t inputIndex() const { return inputIndex_; }

  uint8_t at(uint8_t index) const {
    return index < length_ ? sequence_[index] : 0;
  }

  uint8_t expected() const {
    return inputIndex_ < length_ ? sequence_[inputIndex_] : 0;
  }

  static uint8_t nextLight(uint8_t current) {
    return static_cast<uint8_t>((normalize(current) + 1) % kLightCount);
  }

  static uint8_t previousLight(uint8_t current) {
    return static_cast<uint8_t>(
        (normalize(current) + kLightCount - 1) % kLightCount);
  }

 private:
  static uint8_t normalize(uint8_t light) {
    return static_cast<uint8_t>(light % kLightCount);
  }

  uint8_t sequence_[kMaxLength] = {};
  uint8_t length_ = 0;
  uint8_t inputIndex_ = 0;
};

}  // namespace mikey
