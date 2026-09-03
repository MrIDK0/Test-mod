#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

// All the "customizable in the code" knobs live here in one place.
namespace AutoClickPadConfig {
    // Best-effort object ID for the Red (S-shaped) Jump Pad. Object IDs are
    // easy to verify yourself: place the object in the level editor, select
    // it, and its ID shows in the object info panel - adjust this constant
    // to match if it's off, or to target a completely different object.
    constexpr int TARGET_OBJECT_ID = 1332;

    // How close the player needs to be (in-game units) to count as "near"
    constexpr float TRIGGER_RADIUS = 31.2f;

    // Which input this simulates - 1 is jump / the primary click
    constexpr int CLICK_BUTTON = 1;

    // How many frames the click should be held down for.
    // Set to 2 by default, but you can change this to any integer value.
    constexpr int CLICK_DURATION_FRAMES = 2;
}

namespace AutoClickPad {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-pad-enabled", false);
    }
}
