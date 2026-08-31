#include <Geode/modify/PlayLayer.hpp>
#include "SpeedHack.hpp"

using namespace geode::prelude;

class $modify(MySpeedHackPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        SpeedHack::apply();

        return true;
    }

    void onExit() {
        // Always leave the game at normal speed once you leave the level,
        // so it doesn't affect menus or other layers.
        SpeedHack::reset();
        PlayLayer::onExit();
    }
};
