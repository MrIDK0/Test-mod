#include <Geode/modify/GJBaseGameLayer.hpp>
#include "AutoClickPad.hpp"

using namespace geode::prelude;

// Resets the "already clicked" debounce once per physics tick, so a single
// pad touch only ever produces one click even if more than one callback
// happens to fire for it in the same tick.
class $modify(MyAutoClickPadReset, GJBaseGameLayer) {
    void update(float dt) {
        AutoClickPad::alreadyClickedThisTick() = false;
        GJBaseGameLayer::update(dt);
    }
};
