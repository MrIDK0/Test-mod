#pragma once
#include <Geode/loader/Mod.hpp>
#include <cstddef>

using namespace geode::prelude;

// All the "customizable in the code" knobs live here in one place.
namespace AutoClickPadConfig {
    // Object IDs this feature reacts to - add or remove entries to support
    // more (or fewer) pad types. Object IDs are easy to verify yourself:
    // place the object in the level editor, select it, and its ID shows in
    // the object info panel - these are best-effort defaults, adjust any
    // that don't match your GD version.
    constexpr int TARGET_OBJECT_IDS[] = {
        35,    // Yellow Jump Pad
        67,    // Pink (Purple) Jump Pad
        1332,  // Red (S-shaped) Jump Pad
       140, // Add more object IDs here as needed
    };
    constexpr std::size_t TARGET_OBJECT_ID_COUNT =
        sizeof(TARGET_OBJECT_IDS) / sizeof(TARGET_OBJECT_IDS[0]);

    // Which input this simulates - 1 is jump / the primary click
    constexpr int CLICK_BUTTON = 1;

    inline bool isTargetPad(int objectID) {
        for (std::size_t i = 0; i < TARGET_OBJECT_ID_COUNT; i++) {
            if (TARGET_OBJECT_IDS[i] == objectID) return true;
        }
        return false;
    }
}

namespace AutoClickPad {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-pad-enabled", false);
    }
}
