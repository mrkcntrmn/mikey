#pragma once

#include <stdint.h>

#include "../core/Activity.h"
#include "DiceSession.h"

namespace mikey {

// Deterministic *examples*, not a random generator or measured probabilities.
// A fair die is not guaranteed to produce every face in a small sample.
class DiceGuidedSession {
 public:
  enum class Phase : uint8_t { Predict = 0, Observe, Explain, Complete };

  struct Question {
    const char* question;
    bool correctYes;
    const char* example;
    uint8_t exampleFace;
    const char* explanation;
  };

  void reset(ActivityMode mode) {
    mode_ = mode;
    index_ = 0;
    selectedYes_ = false;
    retry_ = false;
    phase_ = Phase::Predict;
  }
  Phase phase() const { return phase_; }
  ActivityMode mode() const { return mode_; }
  uint8_t questionCount() const {
    return mode_ == ActivityMode::Learn ? kLearnCount : kChallengeCount;
  }
  uint8_t questionIndex() const { return index_; }
  bool selectedYes() const { return selectedYes_; }
  bool retry() const { return retry_; }

  const Question& question() const {
    const uint8_t bounded = index_ < questionCount()
        ? index_ : static_cast<uint8_t>(questionCount() - 1);
    return mode_ == ActivityMode::Learn ? kLearn[bounded] : kChallenge[bounded];
  }

  bool rotate(int8_t direction) {
    if (direction == 0 || phase_ != Phase::Predict) return false;
    selectedYes_ = !selectedYes_;
    retry_ = false;
    return true;
  }

  void submit() {
    switch (phase_) {
      case Phase::Predict:
        if (selectedYes_ != question().correctYes) {
          retry_ = true;
          return;
        }
        retry_ = false;
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
          selectedYes_ = false;
          retry_ = false;
          phase_ = Phase::Predict;
        }
        return;
      case Phase::Complete:
        reset(mode_);
        return;
    }
  }

 private:
  // Answers alternate YES/NO where possible; every example is deliberately
  // fixed and explicitly labeled SAMPLE, never a live or lucky random roll.
  inline static constexpr Question kLearn[] = {
      {"CAN 4 REPEAT?", true, "SAMPLE: 4 4", 4, "SAME FACE CAN REPEAT"},
      {"MUST 6 BE NEXT?", false, "SAMPLE: 1 1", 1, "NO FACE IS DUE"},
      {"ALL FACES IN 6?", false, "SAMPLE: 2 2 2", 2, "SIX MAY REPEAT"},
  };
  inline static constexpr Question kChallenge[] = {
      {"CAN 1 FOLLOW 1?", true, "SAMPLE: 1 1", 1, "REPEATS ARE POSSIBLE"},
      {"IS 6 DUE NEXT?", false, "SAMPLE: 3 3", 3, "ROLLS DO NOT REMEMBER"},
      {"ALWAYS EVEN?", false, "SAMPLE: 5 5 1", 5, "SMALL RUNS VARY"},
      {"CAN 3 BE NEXT?", true, "SAMPLE: 6 3", 3, "ANY FACE IS POSSIBLE"},
  };
  static constexpr uint8_t kLearnCount =
      static_cast<uint8_t>(sizeof(kLearn) / sizeof(kLearn[0]));
  static constexpr uint8_t kChallengeCount =
      static_cast<uint8_t>(sizeof(kChallenge) / sizeof(kChallenge[0]));

  ActivityMode mode_ = ActivityMode::Learn;
  Phase phase_ = Phase::Predict;
  uint8_t index_ = 0;
  bool selectedYes_ = false;
  bool retry_ = false;
};

}  // namespace mikey
