#pragma once

#include <stdint.h>

#include "DiceSession.h"

namespace mikey {

// Two distinguishable six-sided dice. Each completed roll stores one ordered
// pair and one sum. Animation frames do not count as completed rolls.
class DualDiceSession {
 public:
  static constexpr uint8_t kMinTotal = 2;
  static constexpr uint8_t kMaxTotal = 12;

  void reset() {
    rolls_ = 0;
    doubles_ = 0;
    for (uint8_t i = 0; i <= kMaxTotal; ++i) totals_[i] = 0;
    for (uint8_t i = 0; i < DiceSession::kFaceCount; ++i) {
      leftCounts_[i] = 0;
      rightCounts_[i] = 0;
    }
  }

  static bool validFace(uint8_t face) {
    return face >= 1 && face <= DiceSession::kFaceCount;
  }

  static uint8_t faceFromRandomIndex(uint8_t index) {
    return DiceSession::faceFromRandomIndex(index);
  }

  static uint8_t sum(uint8_t left, uint8_t right) {
    return validFace(left) && validFace(right)
        ? static_cast<uint8_t>(left + right) : 0;
  }

  // Count equally likely *ordered* pairs; does not infer probabilities from
  // a small observed sample.
  static uint8_t waysForTotal(uint8_t total) {
    if (total < kMinTotal || total > kMaxTotal) return 0;
    return total <= 7 ? static_cast<uint8_t>(total - 1)
                      : static_cast<uint8_t>(13 - total);
  }

  bool record(uint8_t left, uint8_t right) {
    if (!validFace(left) || !validFace(right)) return false;
    ++rolls_;
    ++totals_[left + right];
    ++leftCounts_[left - 1];
    ++rightCounts_[right - 1];
    if (left == right) ++doubles_;
    return true;
  }

  uint32_t totalRolls() const { return rolls_; }
  uint32_t doublesCount() const { return doubles_; }
  uint32_t countForTotal(uint8_t total) const {
    return total >= kMinTotal && total <= kMaxTotal ? totals_[total] : 0;
  }
  uint32_t countForLeft(uint8_t face) const {
    return validFace(face) ? leftCounts_[face - 1] : 0;
  }
  uint32_t countForRight(uint8_t face) const {
    return validFace(face) ? rightCounts_[face - 1] : 0;
  }

 private:
  uint32_t totals_[kMaxTotal + 1] = {};
  uint32_t leftCounts_[DiceSession::kFaceCount] = {};
  uint32_t rightCounts_[DiceSession::kFaceCount] = {};
  uint32_t rolls_ = 0;
  uint32_t doubles_ = 0;
};

}  // namespace mikey
