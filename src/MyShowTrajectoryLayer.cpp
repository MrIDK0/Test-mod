#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "Trajectory.hpp"

using namespace geode::prelude;

// Core technique: rather than reimplementing GD's physics formulas
// ourselves (which would mean guessing at exact, undocumented per-mode
// constants), this spawns a real, invisible PlayerObject, copies the real
// player's live state onto it, then repeatedly calls the game's OWN real
// update() and checkCollisions() functions to fast-forward. The prediction
// is exactly as accurate as GD's actual physics, because it IS GD's actual
// physics - just run ahead on a throwaway copy.
//
// A fresh dummy is created and destroyed for every single simulation run,
// rather than reused across frames - reuse caused it to freeze after the
// first run, almost certainly because a predicted death leaves some
// internal "is dead"-style state on the object that a plain state-copy
// doesn't reliably clear. A brand new object every time sidesteps that
// entirely instead of chasing down the exact flag.
namespace {
    bool isSimulating = false;
    float lastRealDt = 1.f / 60.f;
    CCDrawNode* trajectoryDrawNode = nullptr;

    CCDrawNode* createTrajectoryDrawNode(CCLayer* layer, CCNode* debugNode) {
        auto parent = debugNode ? debugNode->getParent() : layer;
        if (!parent) return nullptr;
        auto node = CCDrawNode::create();
        node->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        node->setZOrder(99999);
        node->setID("trajectory-node"_spr);
        parent->addChild(node);
        return node;
    }

    PlayerObject* createDummyPlayer(PlayLayer* layer) {
        if (!layer) return nullptr;
        auto dummy = PlayerObject::create(1, 1, layer, layer, true);
        if (dummy) {
            dummy->setVisible(false);
            dummy->setID("trajectory-dummy"_spr);
            if (layer->m_objectLayer) {
                layer->m_objectLayer->addChild(dummy);
            }
        }
        return dummy;
    }

    // Copies the live player's state onto the throwaway dummy. copyAttributes
    // handles most of it; the explicit fields below are re-asserted as a
    // safety net for the specific values the simulation depends on most.
    void copyPlayerState(PlayerObject* from, PlayerObject* to) {
        to->copyAttributes(from);
        to->setPosition(from->getPosition());
        to->m_isOnGround   = from->m_isOnGround;
        to->m_gravityMod   = from->m_gravityMod;
        to->m_isSideways   = from->m_isSideways;
        to->m_isDashing    = from->m_isDashing;
        to->m_dashX        = from->m_dashX;
        to->m_dashY        = from->m_dashY;
        to->m_dashAngle    = from->m_dashAngle;
        to->m_yVelocity    = from->m_yVelocity;
        to->m_isShip       = from->m_isShip;
        to->m_isBall       = from->m_isBall;
        to->m_isBird       = from->m_isBird;
        to->m_isDart       = from->m_isDart;
        to->m_isRobot      = from->m_isRobot;
        to->m_isSpider     = from->m_isSpider;
    }

    void simulatePath(PlayLayer* layer, PlayerObject* source, bool holdJump) {
        if (!layer || !source || !trajectoryDrawNode) return;

        auto sim = createDummyPlayer(layer);
        if (!sim) return;

        copyPlayerState(source, sim);
        sim->setVisible(false);

        if (holdJump) {
            sim->pushButton(PlayerButton::Jump);
        } else {
            sim->releaseButton(PlayerButton::Jump);
        }

        auto color = holdJump ? Trajectory::getJumpColor() : Trajectory::getNormalColor();
        float thickness = Trajectory::getLineThickness();
        int iterations = Trajectory::getIterations();

        for (int i = 0; i < iterations; i++) {
            CCPoint before = sim->getPosition();

            sim->update(lastRealDt);
            int result = layer->checkCollisions(sim, lastRealDt, false);

            trajectoryDrawNode->drawSegment(before, sim->getPosition(), thickness, color);

            if (result == 1) {
                trajectoryDrawNode->drawDot(sim->getPosition(), thickness * 3.f, Trajectory::getDeathColor());
                break;
            }
        }

        sim->removeFromParentAndCleanup(true);
    }

    void runTrajectorySimulation(PlayLayer* layer) {
        if (!trajectoryDrawNode) return;
        trajectoryDrawNode->clear();

        if (!Trajectory::isEnabled()) return;

        isSimulating = true;

        if (layer->m_player1) {
            simulatePath(layer, layer->m_player1, true);
            simulatePath(layer, layer->m_player1, false);
        }
        if (layer->m_gameState.m_isDualMode && layer->m_player2) {
            simulatePath(layer, layer->m_player2, true);
            simulatePath(layer, layer->m_player2, false);
        }

        isSimulating = false;
    }
}

class $modify(MyTrajectoryPlayLayer, PlayLayer) {
    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        trajectoryDrawNode = createTrajectoryDrawNode(this, m_debugDrawNode);
    }

    void updateVisibility(float dt) {
        PlayLayer::updateVisibility(dt);
        if (!isSimulating) {
            runTrajectorySimulation(this);
        }
    }

    void resetLevel() {
        trajectoryDrawNode = nullptr;
        isSimulating = false;
        PlayLayer::resetLevel();
    }
};

class $modify(MyTrajectoryPlayerObject, PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);
        if (!isSimulating) {
            lastRealDt = dt;
        }
    }

    // Suppresses a visual dash effect from firing during simulation - the
    // dummy players are invisible, but some effects spawn independent
    // particle nodes that would otherwise flash briefly regardless.
    void playSpiderDashEffect(CCPoint from, CCPoint to) {
        if (isSimulating) return;
        PlayerObject::playSpiderDashEffect(from, to);
    }
};

class $modify(MyTrajectoryGJBaseGameLayer, GJBaseGameLayer) {
    // Prevents the simulation from actually destroying real breakable
    // blocks in the level - without this, predicting a path through a
    // breakable would break it for real, for everyone watching.
    void destroyObject(GameObject* object) {
        if (isSimulating && object && object->m_objectType == GameObjectType::Breakable) {
            return;
        }
        GJBaseGameLayer::destroyObject(object);
    }
};
