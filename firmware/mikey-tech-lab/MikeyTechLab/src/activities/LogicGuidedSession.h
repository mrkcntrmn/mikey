#pragma once

#include <stdint.h>

#include "../core/Activity.h"
#include "LogicState.h"

namespace mikey {

// Pure, hardware-independent guide to predicting Boolean outputs.
// Reuses the already-tested LogicState truth table rather than a second rule engine.
class LogicGuidedSession {
 public:
  enum class Phase : uint8_t { Predict = 0, Observe, Explain, Complete };
  struct Question {
    LogicRule rule;
    bool a;
    bool b;
    const char* explanation;
  };

  void reset(ActivityMode mode) {
    mode_ = mode;
    index_ = 0;
    selectedOutput_ = false;
    wrongAttempt_ = false;
    phase_ = Phase::Predict;
  }

  ActivityMode mode() const { return mode_; }
  Phase phase() const { return phase_; }
  uint8_t questionIndex() const { return index_; }
  uint8_t questionCount() const {
    return mode_ == ActivityMode::Learn ? kLearnCount : kChallengeCount;
  }
  bool selectedOutput() const { return selectedOutput_; }
  bool wrongAttempt() const { return wrongAttempt_; }

  const Question& question() const {
    const uint8_t index =
        index_ < questionCount() ? index_ : static_cast<uint8_t>(questionCount() - 1);
    return mode_ == ActivityMode::Learn ? kLearn[index] : kChallenge[index];
  }

  // Drive the same model used by Logic Lab PLAY, including NOT A ignoring B.
  bool expectedOutput() const {
    const Question& q = question();
    LogicState state;
    state.reset();
    if (q.a) state.toggleA();
    if (q.b) state.toggleB();
    for (uint8_t i = 0; i < LogicState::kRuleCount; ++i) {
      if (state.rule() == q.rule) return state.output();
      state.nextRule();
    }
    return false;
  }

  bool rotate(int8_t direction) {
    if (direction == 0 || phase_ != Phase::Predict) return false;
    selectedOutput_ = !selectedOutput_;
    wrongAttempt_ = false;
    return true;
  }

  void submit() {
    switch (phase_) {
      case Phase::Predict:
        if (selectedOutput_ != expectedOutput()) {
          wrongAttempt_ = true;
          return;  // Do not advance or reveal inputs/outputs through LEDs.
        }
        wrongAttempt_ = false;
        phase_ = Phase::Observe;
        return;
      case Phase::Observe:
        phase_ = Phase::Explain;
        return;
      case Phase::Explain:
        ++index_;
        if (index_ >= questionCount()) {
          phase_ = Phase::Complete;
        } else {
          phase_ = Phase::Predict;
          selectedOutput_ = false;
          wrongAttempt_ = false;
        }
        return;
      case Phase::Complete:
        reset(mode_);
        return;
    }
  }

 private:
  inline static constexpr Question kLearn[] = {
      {LogicRule::And, true, false, "AND NEEDS BOTH"},
      {LogicRule::Or, true, false, "OR NEEDS EITHER"},
      {LogicRule::NotA, false, true, "NOT A FLIPS A"},
  };
  inline static constexpr Question kChallenge[] = {
      {LogicRule::And, true, true, "BOTH INPUTS ON"},
      {LogicRule::Or, false, false, "NEITHER INPUT ON"},
      {LogicRule::NotA, true, true, "A IS ON; NOT = 0"},
      {LogicRule::And, false, true, "AND NEEDS BOTH"},
      {LogicRule::Or, false, true, "B ALONE IS ENOUGH"},
      {LogicRule::NotA, false, false, "A OFF; NOT = 1"},
  };
  static constexpr uint8_t kLearnCount =
      static_cast<uint8_t>(sizeof(kLearn) / sizeof(kLearn[0]));
  static constexpr uint8_t kChallengeCount =
      static_cast<uint8_t>(sizeof(kChallenge) / sizeof(kChallenge[0]));

  ActivityMode mode_ = ActivityMode::Learn;
  Phase phase_ = Phase::Predict;
  uint8_t index_ = 0;
  bool selectedOutput_ = false;
  bool wrongAttempt_ = false;
};

}  // namespace mikey
