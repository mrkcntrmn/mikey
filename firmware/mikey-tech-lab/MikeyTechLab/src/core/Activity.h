#pragma once

#include <stdint.h>

#include "InputEvent.h"

namespace mikey {

enum class ActivityId : uint8_t {
  Jackpot = 1,
  Reaction,
  Simon,
  Dice,
  Binary,
  Logic,
  Debugging,
};

enum class ActivityMode : uint8_t {
  Play = 0,
  Learn,
  Challenge,
};

struct ActivityContext {
  ActivityMode mode = ActivityMode::Play;
  uint8_t level = 0;
  bool predictionRequired = false;
};

class Activity {
 public:
  virtual ~Activity() = default;

  virtual ActivityId id() const = 0;
  virtual const char* name() const = 0;

  virtual void begin(const ActivityContext& context) = 0;
  virtual void handleInput(const InputFrame& input) = 0;
  virtual void update() = 0;
  virtual void end() = 0;
  virtual bool exitRequested() const = 0;
};

}  // namespace mikey
