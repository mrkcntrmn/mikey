#include "ModeMenu.h"

#include <stdio.h>

namespace mikey {

void ModeMenu::render(const AppState& state) const {
  const uint8_t index = modeIndex(state.selectedMode);
  const ModeDescriptor& mode = modeAt(index);
  const char* status = mode.available ? "READY" : "SOON";
  const uint16_t statusColor = mode.available ? kColorGreen : kColorGray;

  char countText[16];
  snprintf(countText, sizeof(countText), "%u OF %u",
           static_cast<unsigned>(index + 1),
           static_cast<unsigned>(kModeCount));

  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("JACKPOT", 28, 2, kColorYellow);
  hardware_.drawCenteredText(mode.name, 90, 3, kColorWhite);
  hardware_.drawCenteredText(status, 128, 2, statusColor);
  hardware_.drawCenteredText("<           >", 158, 1, kColorGray);
  hardware_.drawCenteredText(countText, 182, 1, kColorWhite);
  hardware_.drawCenteredText("PRESS TO START", 206, 1, kColorCyan);
  hardware_.drawCenteredText("HOLD FOR HOME", 224, 1, kColorGray);
  hardware_.presentFrame();
  hardware_.clearLeds();
  hardware_.showLeds();
}

void ModeMenu::renderComingSoon(const char* title) const {
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText(title, 78, 2, kColorWhite);
  hardware_.drawCenteredText("COMING", 118, 3, kColorYellow);
  hardware_.drawCenteredText("SOON", 158, 3, kColorYellow);
  hardware_.presentFrame();
  hardware_.clearLeds();
  hardware_.showLeds();
}

}  // namespace mikey
