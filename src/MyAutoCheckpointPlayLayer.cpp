#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

class $modify(MyAutoCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (this->m_isPracticeMode && this->m_player1 && !this->m_player1->m_isDead) {
            // Force save current state directly into the practice checkpoint array
            auto checkpoint = CheckpointObject::create();
            if (checkpoint) {
                this->m_checkpointArray->addObject(checkpoint);
            }
        }
    }
};
