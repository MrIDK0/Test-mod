#include <Geode/modify/PlayerObject.hpp>
#include <Geode/Enums.hpp>
#include <Geode/binding/RingObject.hpp>
#include "AutoClickPad.hpp"

using namespace geode::prelude;

// Hooks the same real engine functions GD calls when a pad or dash orb
// actually affects the player - propellPlayer for the gravity-flipping
// pad, bumpPlayer for jump pads, startDashing for dash orbs - rather than
// checking distance or hitbox overlap ourselves. This fires on the exact
// real touch, every time, with nothing to guess.
class $modify(MyAutoClickPadPlayer, PlayerObject) {
    void queueJumpClick(float holdSeconds) {
        auto* layer = GJBaseGameLayer::get();
        if (!layer) return;

        // Account for the "flip 2-player controls" setting so the click
        // goes to the correct side in 2-player mode.
        bool isPlayer1 = this->m_isSecondPlayer ^
            GameManager::sharedState()->getGameVariable(GameVar::Flip2PlayerControls);

        // Queue a press immediately, then a release after the given hold
        // duration - GD's own queue handles the exact timing.
        layer->queueButton((int)PlayerButton::Jump, true, isPlayer1, 0.0);
        layer->queueButton((int)PlayerButton::Jump, false, isPlayer1, holdSeconds);

        AutoClickPad::alreadyClickedThisTick() = true;
    }

    void propellPlayer(float yVelocity, bool noEffects, int objectType) {
        if (objectType == AutoClickPadConfig::GRAVITY_PAD_TYPE
            && AutoClickPad::gravityPadsEnabled()
            && !AutoClickPad::alreadyClickedThisTick()) {
            queueJumpClick(AutoClickPad::getClickHoldSeconds());
        }
        PlayerObject::propellPlayer(yVelocity, noEffects, objectType);
    }

    void bumpPlayer(float bumpMod, int objectType, bool noEffects, GameObject* object) {
        if (AutoClickPadConfig::isJumpPadType(objectType)
            && AutoClickPad::jumpPadsEnabled()
            && !AutoClickPad::alreadyClickedThisTick()) {
            queueJumpClick(AutoClickPad::getClickHoldSeconds());
        }
        PlayerObject::bumpPlayer(bumpMod, objectType, noEffects, object);
    }

    void startDashing(DashRingObject* object) {
        if (object && !AutoClickPad::alreadyClickedThisTick()) {
            int id = object->m_objectID;
            if (id == AutoClickPadConfig::GREEN_DASH_ORB_ID && AutoClickPad::jumpPadsEnabled()) {
                queueJumpClick(AutoClickPad::getDashOrbHoldSeconds());
            } else if (id == AutoClickPadConfig::PURPLE_DASH_ORB_ID && AutoClickPad::gravityPadsEnabled()) {
                queueJumpClick(AutoClickPad::getDashOrbHoldSeconds());
            }
        }
        PlayerObject::startDashing(object);
    }
};
