#pragma once

#include <stdint.h>

#include "../core/Activity.h"
#include "SimonSequence.h"

namespace mikey {

// Fixed, hardware-independent sequence predictions for teaching ordered memory.
// We reuse SimonSequence as the authority for stored symbols and positions.
class SimonGuidedSession {
 public:
  enum class Phase : uint8_t { Predict = 0, Observe, Explain, Complete };
  struct Question {
    uint8_t length;
    uint8_t lights[4];
    const char* explanation;
  };

  void reset(ActivityMode mode) {
    mode_ = mode;
    questionIndex_ = 0;
    selectedLight_ = 0;
    wrongAttempt_ = false;
    phase_ = Phase::Predict;
    loadQuestion();
  }

  ActivityMode mode() const { return mode_; }
  Phase phase() const { return phase_; }
  uint8_t questionIndex() const { return questionIndex_; }
  uint8_t questionCount() const {
    return mode_ == ActivityMode::Learn ? kLearnCount : kChallengeCount;
  }
  uint8_t length() const { return sequence_.length(); }
  uint8_t prefixLength() const { return static_cast<uint8_t>(sequence_.length() - 1); }
  uint8_t prefixAt(uint8_t index) const {
    return index < prefixLength() ? sequence_.at(index) : 0;
  }
  uint8_t expectedLight() const {
    return sequence_.at(static_cast<uint8_t>(sequence_.length() - 1));
  }
  uint8_t selectedLight() const { return selectedLight_; }
  bool wrongAttempt() const { return wrongAttempt_; }
  const char* explanation() const { return question().explanation; }

  bool rotate(int8_t direction) {
    if (phase_ != Phase::Predict || direction == 0) return false;
    selectedLight_ = direction > 0 ? SimonSequence::nextLight(selectedLight_)
                                    : SimonSequence::previousLight(selectedLight_);
    wrongAttempt_ = false;
    return true;
  }

  void submit() {
    switch (phase_) {
      case Phase::Predict:
        if (selectedLight_ != expectedLight()) {
          wrongAttempt_ = true;  // Same stored sequence remains available.
          return;
        }
        wrongAttempt_ = false;
        phase_ = Phase::Observe;
        return;
      case Phase::Observe:
        phase_ = Phase::Explain;
        return;
      case Phase::Explain:
        ++questionIndex_;
        if (questionIndex_ >= questionCount()) {
          phase_ = Phase::Complete;
        } else {
          selectedLight_ = 0;
          wrongAttempt_ = false;
          loadQuestion();
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
      {2, {0, 2, 0, 0}, "ORDER IS STORED"},
      {3, {4, 1, 3, 0}, "STEP 3 COMES LAST"},
  };
  inline static constexpr Question kChallenge[] = {
      {3, {3, 1, 4, 0}, "SEQUENCE 4-2-5"},
      {4, {2, 0, 3, 1}, "SEQUENCE 3-1-4-2"},
      {4, {4, 2, 0, 3}, "SEQUENCE 5-3-1-4"},
  };
  static constexpr uint8_t kLearnCount =
      static_cast<uint8_t>(sizeof(kLearn) / sizeof(kLearn[0]));
  static constexpr uint8_t kChallengeCount =
      static_cast<uint8_t>(sizeof(kChallenge) / sizeof(kChallenge[0]));

  const Question& question() const {
    const uint8_t bounded = questionIndex_ < questionCount()
        ? questionIndex_ : static_cast<uint8_t>(questionCount() - 1);
    return mode_ == ActivityMode::Learn ? kLearn[bounded] : kChallenge[bounded];
  }
  void loadQuestion() {
    const Question& q = question();
    sequence_.reset(q.lights[0]);
    for (uint8_t i = 1; i < q.length; ++i) {
      sequence_.append(q.lights[i]);
    }
  }

  SimonSequence sequence_;
  ActivityMode mode_ = ActivityMode::Learn;
  Phase phase_ = Phase::Predict;
  uint8_t questionIndex_ = 0;
  uint8_t selectedLight_ = 0;
  bool wrongAttempt_ = false;
};

}  // namespace mikey
