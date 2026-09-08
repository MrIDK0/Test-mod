#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

class $modify(FrameCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        // Call the original update function so the game updates physics/render state
        PlayLayer::update(dt);

        // Ensure checkpoints are only placed during active Practice Mode gameplay
        if (this->m_isPracticeMode && this->m_player1 && !this->m_player1->m_isDead) {
            this->markCheckpoint();
        }
    }
};

