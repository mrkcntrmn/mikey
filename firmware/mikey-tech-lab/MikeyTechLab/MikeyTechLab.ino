#include "src/activities/JackpotActivity.h"
#include "src/activities/ReactionActivity.h"
#include "src/core/ActivityRegistry.h"
#include "src/core/AppState.h"
#include "src/core/LearningLoop.h"
#include "src/hardware/CrowPanelHardware.h"
#include "src/ui/HomeMenu.h"
#include "src/ui/ModeMenu.h"

using namespace mikey;

static CrowPanelHardware hardware;
static HomeMenu homeMenu(hardware);
static ModeMenu modeMenu(hardware);
static JackpotActivity jackpot(hardware);
static ReactionActivity reaction(hardware);
static Activity* activeActivity = nullptr;
static AppState app;
static LearningLoop learningLoop;
static bool uiDirty = true;

static constexpr uint32_t kComingSoonMs = 900;

static Activity* resolveActivity(ActivityId id) {
  switch (id) {
    case ActivityId::Jackpot:
      return &jackpot;
    case ActivityId::Reaction:
      return &reaction;
    default:
      return nullptr;
  }
}

static void showComingSoon(const char* title) {
  app.comingSoonActive = true;
  app.comingSoonUntilMs = hardware.nowMs() + kComingSoonMs;
  app.comingSoonTitle = title;
  uiDirty = true;
}

static void clearComingSoonIfExpired() {
  if (!app.comingSoonActive) return;
  if (static_cast<int32_t>(hardware.nowMs() - app.comingSoonUntilMs) < 0) return;
  app.comingSoonActive = false;
  app.comingSoonTitle = "";
  uiDirty = true;
}

static void goHome() {
  if (app.screen == AppScreen::Activity && activeActivity != nullptr) {
    activeActivity->end();
    activeActivity = nullptr;
  }
  app.screen = AppScreen::Home;
  app.selectedMode = ActivityMode::Play;
  app.comingSoonActive = false;
  uiDirty = true;
}

static void launchSelectedActivity() {
  const ActivityDescriptor& selected = activityAt(app.selectedActivityIndex);
  Activity* next = resolveActivity(selected.id);
  if (next == nullptr) {
    showComingSoon(selected.name);
    return;
  }

  ActivityContext context;
  context.mode = app.selectedMode;
  context.predictionRequired = false;
  learningLoop.reset(false);

  activeActivity = next;
  app.activeActivity = selected.id;
  app.screen = AppScreen::Activity;
  app.comingSoonActive = false;
  activeActivity->begin(context);
  // Paint the first activity frame immediately. ModeSelect handled this input
  // tick, so Activity::update() would otherwise wait until the next loop.
  activeActivity->update();
  uiDirty = false;
}

static void handleHomeInput(const InputFrame& input) {
  if (input.encoderClockwise) {
    app.selectedActivityIndex =
        static_cast<uint8_t>((app.selectedActivityIndex + 1) % kActivityCount);
    uiDirty = true;
  }
  if (input.encoderCounterClockwise) {
    app.selectedActivityIndex = static_cast<uint8_t>(
        (app.selectedActivityIndex + kActivityCount - 1) % kActivityCount);
    uiDirty = true;
  }

  if (input.encoderShortPress || input.touchPress) {
    const ActivityDescriptor& activity = activityAt(app.selectedActivityIndex);
    if (!activity.available) {
      showComingSoon(activity.name);
      return;
    }
    app.selectedMode = ActivityMode::Play;
    app.screen = AppScreen::ModeSelect;
    uiDirty = true;
  }
}

static void handleModeInput(const InputFrame& input) {
  if (input.encoderLongPress) {
    goHome();
    return;
  }

  if (input.encoderClockwise) {
    const uint8_t index = modeIndex(app.selectedMode);
    app.selectedMode = modeAt(static_cast<uint8_t>((index + 1) % kModeCount)).mode;
    uiDirty = true;
  }
  if (input.encoderCounterClockwise) {
    const uint8_t index = modeIndex(app.selectedMode);
    app.selectedMode =
        modeAt(static_cast<uint8_t>((index + kModeCount - 1) % kModeCount)).mode;
    uiDirty = true;
  }

  if (input.encoderShortPress || input.touchPress) {
    const ActivityDescriptor& activity = activityAt(app.selectedActivityIndex);
    const ModeDescriptor& mode = modeAt(modeIndex(app.selectedMode));
    if (!isModeAvailable(activity.id, mode.mode)) {
      showComingSoon(mode.name);
      return;
    }
    launchSelectedActivity();
  }
}

static void handleActivityInput(const InputFrame& input) {
  if (activeActivity == nullptr) {
    goHome();
    return;
  }

  activeActivity->handleInput(input);
  if (activeActivity->exitRequested()) {
    goHome();
  }
}

static void renderCurrentScreen() {
  if (app.comingSoonActive) {
    if (app.screen == AppScreen::ModeSelect) {
      modeMenu.renderComingSoon(app.comingSoonTitle);
    } else {
      homeMenu.renderComingSoon(app.comingSoonTitle);
    }
    return;
  }

  switch (app.screen) {
    case AppScreen::Home:
      homeMenu.render(app);
      break;
    case AppScreen::ModeSelect:
      modeMenu.render(app);
      break;
    case AppScreen::Activity:
      // Active activity owns its frames while running.
      break;
  }
}

void setup() {
  hardware.begin();
  app.screen = AppScreen::Home;
  app.selectedActivityIndex = 0;
  app.selectedMode = ActivityMode::Play;
  activeActivity = nullptr;
  uiDirty = true;
  renderCurrentScreen();
  Serial.println("Mikey Tech Lab app shell ready.");
}

void loop() {
  clearComingSoonIfExpired();

  const InputFrame input = hardware.pollInput();

  if (app.comingSoonActive) {
    // Ignore navigation while the notice is visible.
  } else {
    switch (app.screen) {
      case AppScreen::Home:
        handleHomeInput(input);
        break;
      case AppScreen::ModeSelect:
        handleModeInput(input);
        break;
      case AppScreen::Activity:
        handleActivityInput(input);
        if (activeActivity != nullptr) {
          activeActivity->update();
        }
        break;
    }
  }

  if (uiDirty && app.screen != AppScreen::Activity) {
    renderCurrentScreen();
    uiDirty = false;
  } else if (uiDirty && app.comingSoonActive) {
    renderCurrentScreen();
    uiDirty = false;
  }

  delay(1);
}
