#pragma once
#include <Geode/loader/Mod.hpp>
#include <cstddef>

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
}

namespace AutoClickPad {
    inline bool jumpPadsEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-jumppads-enabled", false);
    }
    inline bool gravityPadsEnabled() {
        return Mod::get()->getSavedValue<bool>("autoclick-gravitypads-enabled", false);
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
