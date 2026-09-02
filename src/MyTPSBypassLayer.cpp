#include <Geode/modify/GJBaseGameLayer.hpp>
#include "TPSBypass.hpp"

using namespace geode::prelude;

// Hooked on GJBaseGameLayer (not PlayLayer) since that's the actual class
// that owns update() - PlayLayer inherits it, so this covers normal
// levels, and also the editor's test mode for free.
//
// Note: overriding getModifiedDelta()'s return value alone doesn't reliably
// work - other internal physics state stays based on the real frame time,
// so it falls out of sync. Instead, feed the desired timestep in as the
// input to update() itself, so everything downstream (including GD's own
// internal getModifiedDelta call) works consistently off it.
class $modify(MyTPSBypassLayer, GJBaseGameLayer) {
    void update(float dt) {
        if (TPSBypass::isEnabled()) {
            GJBaseGameLayer::update(TPSBypass::getDesiredDelta());
        } else {
            GJBaseGameLayer::update(dt);
        }
    }
};
