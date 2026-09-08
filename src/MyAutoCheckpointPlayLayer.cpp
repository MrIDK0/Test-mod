#include <Geode/Geode.hpp>
#include "AutoCheckpoint.hpp"
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);

        auto pl = PlayLayer::get();
        // Check if we are in PlayLayer, in practice mode, and this is player 1
        if (pl && pl->m_isPracticeMode && !this->m_isDead && pl->m_player1 == this) {
            pl->createCheckpoint();
        }
    }
};
