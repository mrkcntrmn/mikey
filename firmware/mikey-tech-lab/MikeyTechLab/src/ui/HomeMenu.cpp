#include "HomeMenu.h"

#include <stdio.h>

namespace mikey {

void HomeMenu::render(const AppState& state) const {
  const ActivityDescriptor& activity = activityAt(state.selectedActivityIndex);
  const char* status = activity.available ? "READY" : "SOON";
  const uint16_t statusColor = activity.available ? kColorGreen : kColorGray;

  char indexText[8];
  snprintf(indexText, sizeof(indexText), "%02u",
           static_cast<unsigned>(state.selectedActivityIndex + 1));

  char countText[16];
  snprintf(countText, sizeof(countText), "%u OF %u",
           static_cast<unsigned>(state.selectedActivityIndex + 1),
           static_cast<unsigned>(kActivityCount));

  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText("TECH LAB", 15, 1, kColorYellow);
  hardware_.drawCenteredText("MIKEY", 42, 3, kColorYellow);
  hardware_.drawCenteredText(indexText, 75, 2, kColorCyan);
  hardware_.drawCenteredText(activity.name, 111, 3, kColorWhite);
  hardware_.drawCenteredText(status, 145, 2, statusColor);
  hardware_.drawCenteredText("<           >", 174, 1, kColorGray);
  hardware_.drawCenteredText(countText, 198, 1, kColorWhite);
  hardware_.drawCenteredText("PRESS TO SELECT", 220, 1, kColorCyan);
  hardware_.presentFrame();
  hardware_.clearLeds();
  hardware_.showLeds();
}

void HomeMenu::renderComingSoon(const char* title) const {
  hardware_.clearFrame(kColorBlack);
  hardware_.drawCenteredText(title, 78, 2, kColorWhite);
  hardware_.drawCenteredText("COMING", 118, 3, kColorYellow);
  hardware_.drawCenteredText("SOON", 158, 3, kColorYellow);
  hardware_.presentFrame();
  hardware_.clearLeds();
  hardware_.showLeds();
}

}  // namespace mikey
