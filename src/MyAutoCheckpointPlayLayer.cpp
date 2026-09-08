#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

class $modify(FrameCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (this->m_isPracticeMode && this->m_player1 && !this->m_player1->m_isDead) {
            // Directly dispatch the practice checkpoint action
            this->handleButton(true, static_cast<int>(PlayerButton::PlaceCheckpoint), true);
            this->handleButton(false, static_cast<int>(PlayerButton::PlaceCheckpoint), true);
        }
    }
};
