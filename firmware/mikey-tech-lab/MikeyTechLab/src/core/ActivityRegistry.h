#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Activity.h"

// Explicit opt-in for device-only physical acceptance. Normal builds stay SOON.
#ifndef MIKEY_ENABLE_BINARY_GUIDED_PREVIEW
#define MIKEY_ENABLE_BINARY_GUIDED_PREVIEW 0
#endif
#if MIKEY_ENABLE_BINARY_GUIDED_PREVIEW != 0 && MIKEY_ENABLE_BINARY_GUIDED_PREVIEW != 1
#error "MIKEY_ENABLE_BINARY_GUIDED_PREVIEW must be 0 or 1"
#endif

// Independent opt-in. The normal build keeps Logic guided modes SOON.
#ifndef MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW
#define MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW 0
#endif
#if MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW != 0 && MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW != 1
#error "MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW must be 0 or 1"
#endif

// Simon guided pilot is independently disabled until physical acceptance.
#ifndef MIKEY_ENABLE_SIMON_GUIDED_PREVIEW
#define MIKEY_ENABLE_SIMON_GUIDED_PREVIEW 0
#endif
#if MIKEY_ENABLE_SIMON_GUIDED_PREVIEW != 0 && MIKEY_ENABLE_SIMON_GUIDED_PREVIEW != 1
#error "MIKEY_ENABLE_SIMON_GUIDED_PREVIEW must be 0 or 1"
#endif

namespace mikey {

struct ActivityDescriptor {
  ActivityId id;
  const char* name;
  bool available;
};

inline constexpr ActivityDescriptor kActivityRegistry[] = {
    {ActivityId::Jackpot, "JACKPOT", true},
    {ActivityId::Reaction, "REACTION", true},
    {ActivityId::Simon, "SIMON", true},
    {ActivityId::Dice, "DICE", true},
    {ActivityId::Binary, "BINARY", true},
    {ActivityId::Logic, "LOGIC", true},
    {ActivityId::Debugging, "DEBUG", true},
};

inline constexpr size_t kActivityCount =
    sizeof(kActivityRegistry) / sizeof(kActivityRegistry[0]);

inline const ActivityDescriptor& activityAt(uint8_t index) {
  return kActivityRegistry[index % kActivityCount];
}

struct ModeDescriptor {
  ActivityMode mode;
  const char* name;
};

inline constexpr ModeDescriptor kModes[] = {
    {ActivityMode::Play, "PLAY"},
    {ActivityMode::Learn, "LEARN"},
    {ActivityMode::Challenge, "CHALLENGE"},
};

inline constexpr size_t kModeCount = sizeof(kModes) / sizeof(kModes[0]);

inline const ModeDescriptor& modeAt(uint8_t index) {
  return kModes[index % kModeCount];
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

inline bool isModeAvailable(ActivityId activity, ActivityMode mode) {
  switch (activity) {
    case ActivityId::Jackpot:
    case ActivityId::Reaction:
    case ActivityId::Dice:
    case ActivityId::Debugging:
      return mode == ActivityMode::Play;
    case ActivityId::Binary:
      return mode == ActivityMode::Play ||
             (MIKEY_ENABLE_BINARY_GUIDED_PREVIEW &&
              (mode == ActivityMode::Learn || mode == ActivityMode::Challenge));
    case ActivityId::Logic:
      return mode == ActivityMode::Play ||
             (MIKEY_ENABLE_LOGIC_GUIDED_PREVIEW &&
              (mode == ActivityMode::Learn || mode == ActivityMode::Challenge));
    case ActivityId::Simon:
      return mode == ActivityMode::Play ||
             (MIKEY_ENABLE_SIMON_GUIDED_PREVIEW &&
              (mode == ActivityMode::Learn || mode == ActivityMode::Challenge));
    default:
      return false;
  }
}

}  // namespace mikey
