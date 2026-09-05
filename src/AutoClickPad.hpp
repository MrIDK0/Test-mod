#pragma once
#include <Geode/loader/Mod.hpp>
#include <cstddef>
#include <string>

using namespace geode::prelude;

// The values below are "objectType" tags GD itself passes into
// PlayerObject::bumpPlayer / propellPlayer when a pad affects the player -
// NOT level editor object IDs (those are a completely different numbering).
// This is the actual mechanism the engine uses to react to pads, so hooking
// off these means there's no proximity/hitbox guessing involved at all.
namespace AutoClickPadConfig {
    // objectType values seen in bumpPlayer for the yellow/pink/red jump pads
    constexpr int JUMP_PAD_TYPES[] = { 8, 9, 34 };
    constexpr std::size_t JUMP_PAD_TYPE_COUNT =
        sizeof(JUMP_PAD_TYPES) / sizeof(JUMP_PAD_TYPES[0]);

    // objectType value seen in propellPlayer for the gravity-flipping (blue) pad
    constexpr int GRAVITY_PAD_TYPE = 10;

    inline bool isJumpPadType(int objectType) {
        for (std::size_t i = 0; i < JUMP_PAD_TYPE_COUNT; i++) {
            if (JUMP_PAD_TYPES[i] == objectType) return true;
        }
        return false;
    }

    // Dash orbs use a different signal entirely - PlayerObject::startDashing
    // is called with the actual DashRingObject touched, so these are real
    // level-editor object IDs (a different numbering than the objectType
    // tags above). Best-effort defaults - verify in the editor's object
    // info panel and adjust if either doesn't match.
    constexpr int GREEN_DASH_ORB_ID  = 1704;
    constexpr int PURPLE_DASH_ORB_ID = 1751;

    // Fixed hold for dash-orb triggers specifically, independent of the
    // user-configurable Click Frames field (which only applies to pads).
    constexpr int DASH_ORB_HOLD_FRAMES = 2;
    inline float getDashOrbHoldSeconds() {
        return static_cast<float>(DASH_ORB_HOLD_FRAMES) / 240.f;
    }
}

namespace AutoClickPad {
    inline bool jumpPadsEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-jumppads-enabled", false);
    }
    inline bool gravityPadsEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-gravitypads-enabled", false);
    }

    // How many frames to hold the click for, same pattern as Speed Hack's
    // text input - saved as a string, parsed and clamped here.
    inline int getClickFrames() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("autoclick-pad-frames", "1");
        int frames = 1;
        try {
            frames = std::stoi(valueStr);
        } catch (...) {
            frames = 1;
        }
        if (frames < 1) frames = 1;
        if (frames > 60) frames = 60; // guard against absurd hold times
        return frames;
    }

    // queueButton's last parameter is a delay (in seconds) before that
    // button event applies - this converts the frame count into that delay,
    // assuming a 240Hz reference tick rate (GD's standard physics rate).
    // If the actual hold time looks off in testing, this is the one
    // conversion to double check.
    inline float getClickHoldSeconds() {
        return static_cast<float>(getClickFrames()) / 240.f;
    }

    // Shared across both the PlayerObject hook and the per-tick reset hook:
    // stays true for the rest of the current physics tick once a click has
    // been queued, so a single pad touch can only ever cause one click even
    // if more than one callback happens to fire for it.
    inline bool& alreadyClickedThisTick() {
        static bool value = false;
        return value;
    }
}
