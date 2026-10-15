#pragma once

#include <stdint.h>

namespace mikey {

enum class LogicRule : uint8_t {
  And = 0,
  Or,
  NotA,
};

class LogicState {
 public:
  static constexpr uint8_t kRuleCount = 3;

  void reset() {
    a_ = false;
    b_ = false;
    rule_ = LogicRule::And;
  }

  void toggleA() { a_ = !a_; }
  void toggleB() { b_ = !b_; }

  bool a() const { return a_; }
  bool b() const { return b_; }
  LogicRule rule() const { return rule_; }

  void nextRule() {
    const uint8_t index = static_cast<uint8_t>(rule_);
    rule_ = static_cast<LogicRule>((index + 1) % kRuleCount);
  }

  void previousRule() {
    const uint8_t index = static_cast<uint8_t>(rule_);
    rule_ = static_cast<LogicRule>((index + kRuleCount - 1) % kRuleCount);
  }

  bool output() const {
    switch (rule_) {
      case LogicRule::And:
        return a_ && b_;
      case LogicRule::Or:
        return a_ || b_;
      case LogicRule::NotA:
        return !a_;
    }
    return false;
  }

 private:
  bool a_ = false;
  bool b_ = false;
  LogicRule rule_ = LogicRule::And;
};

}  // namespace mikey
