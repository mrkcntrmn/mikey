#pragma once

#include <stdint.h>

#include "../core/Activity.h"
#include "DualDiceSession.h"

namespace mikey {

// Questions teach what is possible with TWO independent dice. Samples are
// deliberately fixed examples, never represented as a fresh random roll.
class DiceGuidedSession {
 public:
  enum class Phase : uint8_t { Predict = 0, Observe, Explain, Complete };
  struct Question {
    const char* prompt;
    bool correctYes;
    uint8_t left;
    uint8_t right;
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
    const uint8_t i = index_ < questionCount()
        ? index_ : static_cast<uint8_t>(questionCount() - 1);
    return mode_ == ActivityMode::Learn ? kLearn[i] : kChallenge[i];
  }
  uint8_t exampleTotal() const {
    return DualDiceSession::sum(question().left, question().right);
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
  inline static constexpr Question kLearn[] = {
      {"CAN DICE MATCH?", true, 4, 4, "DOUBLES CAN HAPPEN"},
      {"CAN TOTAL BE 1?", false, 1, 1, "MIN TOTAL IS 2"},
      {"CAN TOTAL BE 7?", true, 3, 4, "3 PLUS 4 IS 7"},
  };
  inline static constexpr Question kChallenge[] = {
      {"MUST TOTAL BE 7?", false, 2, 5, "ANY TOTAL 2 TO 12"},
      {"CAN TOTAL BE 12?", true, 6, 6, "6 PLUS 6 IS 12"},
      {"ALWAYS EVEN SUM?", false, 1, 2, "1 PLUS 2 IS ODD"},
      {"MORE WAYS FOR 7?", true, 3, 4, "SIX WAYS VS ONE"},
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
