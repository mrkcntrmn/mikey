#pragma once

#include <stdint.h>

namespace mikey {

enum class DebugSubmitResult : uint8_t {
  Incorrect = 0,
  CorrectContinue,
  Complete,
};

class DebugSession {
 public:
  static constexpr uint8_t kCaseCount = 5;
  static constexpr uint8_t kOptionCount = 2;

  void reset() {
    caseIndex_ = 0;
    selectedOption_ = 0;
    solvedCount_ = 0;
    complete_ = false;
  }

  uint8_t caseIndex() const { return caseIndex_; }
  uint8_t selectedOption() const { return selectedOption_; }
  uint8_t solvedCount() const { return solvedCount_; }
  bool complete() const { return complete_; }

  void nextOption() {
    selectedOption_ =
        static_cast<uint8_t>((selectedOption_ + 1) % kOptionCount);
  }

  void previousOption() {
    selectedOption_ = static_cast<uint8_t>(
        (selectedOption_ + kOptionCount - 1) % kOptionCount);
  }

  static uint8_t correctOptionForCase(uint8_t index) {
    // Alternate answers so diagnosis cannot be solved by always choosing A.
    static constexpr uint8_t kAnswers[kCaseCount] = {0, 1, 0, 1, 0};
    return index < kCaseCount ? kAnswers[index] : 0;
  }

  bool selectionIsCorrect() const {
    return !complete_ &&
           selectedOption_ == correctOptionForCase(caseIndex_);
  }

  DebugSubmitResult submit() {
    if (complete_ || !selectionIsCorrect()) {
      return DebugSubmitResult::Incorrect;
    }

    ++solvedCount_;
    if (caseIndex_ + 1 >= kCaseCount) {
      complete_ = true;
      return DebugSubmitResult::Complete;
    }

    ++caseIndex_;
    selectedOption_ = 0;
    return DebugSubmitResult::CorrectContinue;
  }

 private:
  uint8_t caseIndex_ = 0;
  uint8_t selectedOption_ = 0;
  uint8_t solvedCount_ = 0;
  bool complete_ = false;
};

}  // namespace mikey
