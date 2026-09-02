#include <Geode/modify/GJBaseGameLayer.hpp>
#include "TPSBypass.hpp"

using namespace geode::prelude;

// Hooked on GJBaseGameLayer (not PlayLayer) since that's the actual class
// that owns getModifiedDelta - PlayLayer inherits it, so this covers normal
// levels, and also the editor's test mode for free.
class $modify(MyTPSBypassLayer, GJBaseGameLayer) {
    float getModifiedDelta(float dt) {
        // Call the original first - it does some internal bookkeeping
        // beyond just returning a number, so we still want that to happen.
        float original = GJBaseGameLayer::getModifiedDelta(dt);

        if (TPSBypass::isEnabled()) {
            return TPSBypass::getDesiredDelta();
        }

        return original;
    }
};
