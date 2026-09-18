#pragma once

#include <stdint.h>

#include "Activity.h"

namespace mikey {

enum class AppScreen : uint8_t {
  Home = 0,
  ModeSelect,
  Activity,
};

struct AppState {
  AppScreen screen = AppScreen::Home;
  uint8_t selectedActivityIndex = 0;
  ActivityMode selectedMode = ActivityMode::Play;
  ActivityId activeActivity = ActivityId::Jackpot;
  bool comingSoonActive = false;
  uint32_t comingSoonUntilMs = 0;
  const char* comingSoonTitle = "";
};

}  // namespace mikey
