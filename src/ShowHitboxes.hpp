#pragma once
#include <Geode/loader/Mod.hpp>
#include <string>

using namespace geode::prelude;

namespace ShowHitboxes {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-enabled", false);
    }

    inline bool trailEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-trail-enabled", false);
    }

    inline bool fillEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-fill-enabled", false);
    }

    inline bool alphaFadeEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-fade-enabled", false);
    }

    // When on, hitboxes/trail stay hidden during normal play and only
    // appear for a few seconds right after death - for inspecting exactly
    // what you touched, without the overlay being on-screen constantly.
    inline bool onlyOnDeathEnabled() {
        return Mod::get()->getSavedValue<bool>("showhitboxes-onlyondeath-enabled", false);
    }

    // How many past positions the trail keeps, same customizable-value
    // pattern as everywhere else in the menu. Capped well below a literal
    // million - that many stored rects would be a real memory/CPU cost for
    // a debug overlay, so this caps at a still-generous but sane ceiling.
    inline int getTrailLength() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("showhitboxes-trail-length", "240");
        int length = 240;
        try {
            length = std::stoi(valueStr);
        } catch (...) {
            length = 240;
        }
        if (length < 1) length = 1;
        if (length > 5000000) length = 5000000; // sane guard
        return length;
    }
}
