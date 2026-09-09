#pragma once
#include <Geode/loader/Mod.hpp>
#include <string>

using namespace geode::prelude;

namespace ShowPosition {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("showpos-enabled", false);
    }

    // How many decimal places to show for each coordinate, same pattern as
    // Speed Hack's value - saved as a string, parsed and clamped here.
    inline int getDecimals() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("showpos-decimals", "0");
        int decimals = 0;
        try {
            decimals = std::stoi(valueStr);
        } catch (...) {
            decimals = 0;
        }
        if (decimals < 0) decimals = 0;
        if (decimals > 1000) decimals = 1000; // sane guard
        return decimals;
    }
}
