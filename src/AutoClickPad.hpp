#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace AutoClickPadConfig {
    constexpr int TARGET_OBJECT_ID = 1332;
    constexpr float TRIGGER_RADIUS = 31.2f;
    constexpr int CLICK_BUTTON = 1;

    // Set how many physics frames the click should be held down for
    constexpr int CLICK_DURATION_FRAMES = 2;
}

namespace AutoClickPad {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-pad-enabled", false);
    }
}
