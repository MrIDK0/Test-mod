#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

// Practice Mode already lets you place a checkpoint manually. This does
// the same thing automatically, every single frame, so wherever you die
// you respawn practically exactly where you were - the densest possible
// safety net. Only does anything in Practice Mode, since normal mode
// doesn't support checkpoints at all.
//
// Note: PlayLayer::createCheckpoint() only builds a checkpoint object and
// returns it - it doesn't register it anywhere on its own. Setting
// m_tryPlaceCheckpoint instead reuses the same flag the manual checkpoint
// keybind sets, so GD's own per-frame processing does the full real thing.
class $modify(MyAutoCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (AutoCheckpoint::isEnabled() && m_isPracticeMode) {
            m_tryPlaceCheckpoint = true;
        }
    }
};
