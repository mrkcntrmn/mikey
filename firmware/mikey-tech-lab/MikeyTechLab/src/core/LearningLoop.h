#pragma once

#include <stdint.h>

namespace mikey {

enum class LearningPhase : uint8_t {
  Predict = 0,
  Act,
  Observe,
  Explain,
  Complete,
};

class LearningLoop {
 public:
  void reset(bool requirePrediction = true) {
    phase_ = requirePrediction ? LearningPhase::Predict : LearningPhase::Act;
  }

  LearningPhase phase() const { return phase_; }

  bool advance() {
    switch (phase_) {
      case LearningPhase::Predict:
        phase_ = LearningPhase::Act;
        return true;
      case LearningPhase::Act:
        phase_ = LearningPhase::Observe;
        return true;
      case LearningPhase::Observe:
        phase_ = LearningPhase::Explain;
        return true;
      case LearningPhase::Explain:
        phase_ = LearningPhase::Complete;
        return true;
      case LearningPhase::Complete:
        return false;
    }
    return false;
  }

 private:
  LearningPhase phase_ = LearningPhase::Predict;
};

}  // namespace mikey
