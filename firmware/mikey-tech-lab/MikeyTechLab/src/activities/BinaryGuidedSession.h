#pragma once

#include <stdint.h>

#include "../core/Activity.h"

namespace mikey {

// Hardware-independent question flow: PREDICT -> OBSERVE -> EXPLAIN.
// A wrong prediction remains on the current question with no early reveal.
class BinaryGuidedSession {
 public:
  enum class Phase : uint8_t { Predict = 0, Observe, Explain, Complete };
  struct Question {
    uint8_t target;
    uint8_t distractor;
    uint8_t correctSlot; // 0 = A, 1 = B
    const char* explanation;
  };

  void reset(ActivityMode mode) {
    mode_ = mode;
    index_ = 0;
    selected_ = 0;
    wrongAttempt_ = false;
    phase_ = Phase::Predict;
  }
  ActivityMode mode() const { return mode_; }
  Phase phase() const { return phase_; }
  bool wrongAttempt() const { return wrongAttempt_; }
  uint8_t questionIndex() const { return index_; }
  uint8_t questionCount() const {
    return mode_ == ActivityMode::Learn ? kLearnCount : kChallengeCount;
  }
  uint8_t selectedSlot() const { return selected_; }

  const Question& question() const {
    const uint8_t bounded = index_ < questionCount()
        ? index_ : static_cast<uint8_t>(questionCount() - 1);
    return mode_ == ActivityMode::Learn ? kLearn[bounded] : kChallenge[bounded];
  }

  uint8_t optionValue(uint8_t slot) const {
    if (slot > 1) return 0;
    return slot == question().correctSlot ? question().target : question().distractor;
  }

  bool rotate(int8_t direction) {
    if (phase_ != Phase::Predict || direction == 0) return false;
    selected_ = static_cast<uint8_t>(1U - selected_);
    wrongAttempt_ = false;
    return true;
  }

  void submit() {
    switch (phase_) {
      case Phase::Predict:
        if (selected_ != question().correctSlot) {
          wrongAttempt_ = true;
          return;
        }
        wrongAttempt_ = false;
        phase_ = Phase::Observe;
        return;
      case Phase::Observe:
        phase_ = Phase::Explain;
        return;
      case Phase::Explain:
        ++index_;
        if (index_ == questionCount()) {
          phase_ = Phase::Complete;
        } else {
          selected_ = 0;
          wrongAttempt_ = false;
          phase_ = Phase::Predict;
        }
        return;
      case Phase::Complete:
        reset(mode_);
        return;
    }
  }

 private:
  inline static constexpr Question kLearn[] = {
      {5, 6, 0, "5 = 4 + 1"},
      {10, 12, 1, "10 = 8 + 2"},
  };
  inline static constexpr Question kChallenge[] = {
      {3, 5, 0, "3 = 2 + 1"},
      {6, 4, 1, "6 = 4 + 2"},
      {9, 8, 0, "9 = 8 + 1"},
      {12, 10, 1, "12 = 8 + 4"},
  };
  static constexpr uint8_t kLearnCount =
      static_cast<uint8_t>(sizeof(kLearn) / sizeof(kLearn[0]));
  static constexpr uint8_t kChallengeCount =
      static_cast<uint8_t>(sizeof(kChallenge) / sizeof(kChallenge[0]));

  ActivityMode mode_ = ActivityMode::Learn;
  Phase phase_ = Phase::Predict;
  uint8_t index_ = 0;
  uint8_t selected_ = 0;
  bool wrongAttempt_ = false;
};

}  // namespace mikey
