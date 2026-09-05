#include <Geode/modify/PlayLayer.hpp>
#include "Noclip.hpp"

using namespace geode::prelude;

// destroyPlayer is the function GD calls the instant a player should die
// from a hazard - skipping it entirely for a given player is the standard
// way to implement noclip, since nothing that would normally kill them
// gets to run.
class $modify(MyNoclipPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool isP1 = (player == m_player1);
        bool isP2 = (player == m_player2);

        if (isP1 && Noclip::p1Enabled()) return;
        if (isP2 && Noclip::p2Enabled()) return;

        PlayLayer::destroyPlayer(player, object);
    }
};
