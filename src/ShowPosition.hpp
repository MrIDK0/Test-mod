#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace ShowPosition {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("showpos-enabled", false);
    }
}
