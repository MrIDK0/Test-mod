#pragma once
#include <Geode/loader/Mod.hpp>
#include <cocos2d.h>

using namespace geode::prelude;

namespace SpeedHack {

    // Reads the saved toggle + value and applies (or clears) the time scale.
    // Safe to call any time - does nothing harmful if called repeatedly.
    inline void apply() {
        bool enabled = Mod::get()->getSavedValue<bool>("speedhack-enabled", false);
        std::string valueStr = Mod::get()->getSavedValue<std::string>("speedhack-value", "1.0");

        float value = 1.0f;
        try {
            value = std::stof(valueStr);
        } catch (...) {
            value = 1.0f;
        }

        // Guard against 0/negative/garbage values freezing or breaking the game
        if (value <= 0.0000001f) value = 0.0000001f;
        if (value > 9999.f) value = 9999.f;

        CCDirector::sharedDirector()->getScheduler()->setTimeScale(enabled ? value : 1.0f);
    }

    inline void reset() {
        CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
    }

}
