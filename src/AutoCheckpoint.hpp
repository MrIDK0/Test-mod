#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace AutoCheckpoint {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("autocheckpoint-enabled", false);
    }
}
