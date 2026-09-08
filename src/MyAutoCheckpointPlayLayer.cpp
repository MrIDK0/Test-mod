#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

class $modify(FrameCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        // Run original game loop update first
        PlayLayer::update(dt);

        // Check if practice mode is active, player exists, and player isn't dead
        if (this->m_isPracticeMode && this->m_player1 && !this->m_player1->m_isDead) {
            // Force checkpoint creation directly through the level manager
            this->createCheckpoint();
        }
    }
};
