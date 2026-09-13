#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace ShowHitboxes {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-enabled", false);
    }
}
