#pragma once
#include <Geode/loader/Mod.hpp>
#include <string>

using namespace geode::prelude;

// TPS Bypass works differently from Speed Hack: instead of scaling how fast
// time passes (CCScheduler's time scale), this overrides the actual physics
// timestep GD's engine uses per tick - i.e. how many simulation steps happen
// per second, not how fast the clock runs.
namespace TPSBypass {

    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("tpsbypass-enabled", false);
    }

    // Returns the per-tick delta time (in seconds) that corresponds to the
    // saved TPS value, e.g. 240 TPS -> 1/240 seconds per tick.
    inline float getDesiredDelta() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("tpsbypass-value", "240");

        float tps = 240.f;
        try {
            tps = std::stof(valueStr);
        } catch (...) {
            tps = 240.f;
        }

        // Guard against 0/negative/absurd values breaking physics entirely
        if (tps < 1.f) tps = 1.f;
        if (tps > 1000.f) tps = 1000.f;

        return 1.f / tps;
    }

}
