#pragma once
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

namespace Noclip {
    inline bool p1Enabled() {
        return Mod::get()->getSavedValue<bool>("noclip-p1-enabled", false);
    }
    inline bool p2Enabled() {
        return Mod::get()->getSavedValue<bool>("noclip-p2-enabled", false);
    }
}
