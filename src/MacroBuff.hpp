#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace MacroBuff {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("macrobuff-enabled", false);
    }
}
