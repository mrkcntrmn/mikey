#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Activity.h"

namespace mikey {

struct ActivityDescriptor {
  ActivityId id;
  const char* name;
  bool available;
};

inline constexpr ActivityDescriptor kActivityRegistry[] = {
    {ActivityId::Jackpot, "JACKPOT", true},
    {ActivityId::Reaction, "REACTION", false},
    {ActivityId::Simon, "SIMON", false},
    {ActivityId::Dice, "DICE", false},
    {ActivityId::Binary, "BINARY", false},
    {ActivityId::Logic, "LOGIC", false},
    {ActivityId::Debugging, "DEBUG", false},
};

inline constexpr size_t kActivityCount =
    sizeof(kActivityRegistry) / sizeof(kActivityRegistry[0]);

inline const ActivityDescriptor& activityAt(uint8_t index) {
  return kActivityRegistry[index % kActivityCount];
}

struct ModeDescriptor {
  ActivityMode mode;
  const char* name;
  bool available;
};

inline constexpr ModeDescriptor kJackpotModes[] = {
    {ActivityMode::Play, "PLAY", true},
    {ActivityMode::Learn, "LEARN", false},
    {ActivityMode::Challenge, "CHALLENGE", false},
};

inline constexpr size_t kModeCount = sizeof(kJackpotModes) / sizeof(kJackpotModes[0]);

inline const ModeDescriptor& modeAt(uint8_t index) {
  return kJackpotModes[index % kModeCount];
}

inline uint8_t modeIndex(ActivityMode mode) {
  switch (mode) {
    case ActivityMode::Play:
      return 0;
    case ActivityMode::Learn:
      return 1;
    case ActivityMode::Challenge:
      return 2;
  }
  return 0;
}

}  // namespace mikey
