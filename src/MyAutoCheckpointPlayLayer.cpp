#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

// Practice Mode already lets you place a checkpoint manually. This does
// the same thing automatically, every single frame, so wherever you die
// you respawn practically exactly where you were - the densest possible
// safety net. Only does anything in Practice Mode, since normal mode
// doesn't support checkpoints at all.
class $modify(MyAutoCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (AutoCheckpoint::isEnabled() && m_isPracticeMode) {
            auto checkpoint = this->createCheckpoint();
            if (checkpoint) {
                this->storeCheckpoint(checkpoint);
            }
        }
    }
};
