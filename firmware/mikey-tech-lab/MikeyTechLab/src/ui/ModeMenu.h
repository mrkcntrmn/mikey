#pragma once

#include "../core/ActivityRegistry.h"
#include "../core/AppState.h"
#include "../hardware/Hardware.h"

namespace mikey {

class ModeMenu {
 public:
  explicit ModeMenu(Hardware& hardware) : hardware_(hardware) {}

  void render(const AppState& state) const;
  void renderComingSoon(const char* title) const;

 private:
  Hardware& hardware_;
};

}  // namespace mikey
