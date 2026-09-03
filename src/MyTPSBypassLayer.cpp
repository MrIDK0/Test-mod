#include <Geode/modify/GJBaseGameLayer.hpp>
#include "TPSBypass.hpp"

using namespace geode::prelude;

// Hooked on GJBaseGameLayer (not PlayLayer) since that's the actual class
// that owns update() - PlayLayer inherits it, so this covers normal
// levels, and also the editor's test mode for free.
//
// Real fixed-timestep approach: accumulate real elapsed time (dt, which
// already reflects Speed Hack's time scale if that's active too) and drain
// it in fixed-size chunks sized to the desired TPS. This changes how many
// physics steps happen per real second WITHOUT changing how much total game
// time passes per real second - unlike just substituting dt directly, which
// only ends up changing overall speed (that's what happened last time).
class $modify(MyTPSBypassLayer, GJBaseGameLayer) {
    struct Fields {
        float accumulator = 0.f;
    };

    void update(float dt) {
        if (!TPSBypass::isEnabled()) {
            GJBaseGameLayer::update(dt);
            m_fields->accumulator = 0.f; // avoid a burst if re-enabled later
            return;
        }

        float tickDt = TPSBypass::getDesiredDelta();
        m_fields->accumulator += dt;

        // Cap catch-up so a lag spike can't trigger a huge burst of steps
        // (spiral of death) - at most 8 ticks get processed in one frame.
        float maxCatchUp = tickDt * 8.f;
        if (m_fields->accumulator > maxCatchUp) {
            m_fields->accumulator = maxCatchUp;
        }

        int steps = 0;
        while (m_fields->accumulator >= tickDt && steps < 8) {
            GJBaseGameLayer::update(tickDt);
            m_fields->accumulator -= tickDt;
            steps++;
        }
    }
};
