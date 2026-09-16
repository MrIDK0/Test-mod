#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "ShowHitboxes.hpp"
#include "RingBuffer.hpp"
#include <array>
#include <cmath>

using namespace geode::prelude;

namespace {
    constexpr ccColor4F DANGER_COLOR       = {1.f, 0.f, 0.f, 1.f};
    constexpr ccColor4F SOLID_COLOR        = {0.f, 0.f, 1.f, 1.f};
    constexpr ccColor4F PASSABLE_COLOR     = {0.f, 1.f, 1.f, 1.f};
    constexpr ccColor4F TRIGGER_COLOR      = {1.f, 0.f, 1.f, 1.f};
    constexpr ccColor4F OTHER_COLOR        = {0.f, 1.f, 0.f, 1.f};
    constexpr ccColor4F PLAYER_COLOR       = {1.f, 0.f, 0.f, 1.f};
    constexpr ccColor4F PLAYER_INNER_COLOR = {0.f, 0.f, 1.f, 1.f};
    constexpr ccColor4F TRAIL_COLOR        = {1.f, 1.f, 0.f, 1.f};
    constexpr ccColor4F TRAIL_INNER_COLOR  = {0.f, 0.f, 1.f, 1.f};
    constexpr float BORDER_WIDTH = 0.4f;
    constexpr float FILL_ALPHA   = 0.4f;

    // How long hitboxes/trail stay visible after death when "Only On
    // Death" is enabled.
    constexpr float DEATH_FREEZE_SECONDS = 3.0f;

    // Trail history - kept at file scope (not per-instance Fields) since it
    // should keep accumulating across the whole play session, independent
    // of any one object's lifetime.
    RingBuffer<CCRect> playerTrail1, playerTrail2;
    RingBuffer<CCRect> playerInnerTrail1, playerInnerTrail2;
    size_t lastTrailLength = 0;

    bool wasDead = false;
    float deathFreezeTimer = 0.f;

    ccColor4F withAlpha(ccColor4F c, float a) {
        return {c.r, c.g, c.b, a};
    }

    // Respects the Fill Hitboxes toggle - fully transparent fill when off,
    // so only the outline shows.
    ccColor4F getFillColor(ccColor4F c) {
        return ShowHitboxes::fillEnabled() ? withAlpha(c, FILL_ALPHA) : ccColor4F{0.f, 0.f, 0.f, 0.f};
    }

    // Parents to the same node the game's OWN internal debug-draw node uses,
    // rather than guessing - this is what keeps the drawing correctly synced
    // to camera/scroll instead of staying fixed in place.
    CCDrawNode* createHitboxDrawNode(CCLayer* layer, CCNode* debugNode, const char* id) {
        auto parent = debugNode ? debugNode->getParent() : layer;
        if (!parent) return nullptr;

        auto drawNode = CCDrawNode::create();
        drawNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        drawNode->setZOrder(9999);
        drawNode->setID(id);
        parent->addChild(drawNode);
        return drawNode;
    }

    // Standard 2D rotation of a rect's 4 corners around its own center -
    // real, universal geometry, not tied to any specific mod. cocos2d
    // rotation is clockwise degrees, hence the negation before converting.
    std::array<CCPoint, 4> getRotatedCorners(const CCRect& rect, float rotationDegrees) {
        CCPoint center = {rect.getMidX(), rect.getMidY()};
        float hw = rect.size.width / 2.f;
        float hh = rect.size.height / 2.f;

        CCPoint local[4] = {
            {-hw, -hh}, {-hw, hh}, {hw, hh}, {hw, -hh}
        };

        float rad = -CC_DEGREES_TO_RADIANS(rotationDegrees);
        float c = cosf(rad);
        float s = sinf(rad);

        std::array<CCPoint, 4> result;
        for (int i = 0; i < 4; i++) {
            float x = local[i].x * c - local[i].y * s;
            float y = local[i].x * s + local[i].y * c;
            result[i] = CCPoint(center.x + x, center.y + y);
        }
        return result;
    }

    void drawRectOutline(CCDrawNode* node, const CCRect& rect, ccColor4F fill, float border, ccColor4F borderColor) {
        CCPoint verts[4] = {
            {rect.getMinX(), rect.getMinY()},
            {rect.getMinX(), rect.getMaxY()},
            {rect.getMaxX(), rect.getMaxY()},
            {rect.getMaxX(), rect.getMinY()}
        };
        node->drawPolygon(verts, 4, fill, border, borderColor);
    }

    // Draws axis-aligned when rotation is ~0 (the common case), or a real
    // rotated box using getRotatedCorners otherwise - covers rotated
    // spikes/blocks placed at an angle in the editor.
    void drawObjectBox(CCDrawNode* node, const CCRect& rect, float rotationDegrees, ccColor4F fill, float border, ccColor4F borderColor) {
        if (std::fabs(rotationDegrees) < 0.01f) {
            drawRectOutline(node, rect, fill, border, borderColor);
            return;
        }
        auto corners = getRotatedCorners(rect, rotationDegrees);
        node->drawPolygon(corners.data(), 4, fill, border, borderColor);
    }

    void drawObjectHitbox(GJBaseGameLayer* layer, CCDrawNode* drawNode, GameObject* obj) {
        if (!obj) return;
        if (obj->m_objectType == GameObjectType::Decoration || !obj->m_isActivated || obj->m_isGroupDisabled)
            return;
        if (obj == layer->m_player1 || obj == layer->m_player2) return;

        float rotation = obj->getRotation();

        switch (obj->m_objectType) {
            case GameObjectType::Solid: {
                bool passable = obj->m_isPassable;
                auto color = passable ? PASSABLE_COLOR : SOLID_COLOR;
                drawObjectBox(drawNode, obj->getObjectRect(), rotation, getFillColor(color), BORDER_WIDTH, color);
                break;
            }
            case GameObjectType::Hazard:
            case GameObjectType::AnimatedHazard: {
                if (obj == layer->m_anticheatSpike) break;
                float radius = std::max(obj->m_scaleX, obj->m_scaleY) * obj->m_objectRadius;
                if (radius > 0.f) {
                    drawNode->drawCircle(obj->getPosition(), radius, getFillColor(DANGER_COLOR), BORDER_WIDTH, DANGER_COLOR, 16);
                } else {
                    drawObjectBox(drawNode, obj->getObjectRect(), rotation, getFillColor(DANGER_COLOR), BORDER_WIDTH, DANGER_COLOR);
                }
                break;
            }
            case GameObjectType::CollisionObject:
                break;
            default: {
                bool isSpeedPortal = obj->m_objectID == 200 || obj->m_objectID == 201 ||
                                      obj->m_objectID == 202 || obj->m_objectID == 203 ||
                                      obj->m_objectID == 1334;
                if (obj->m_objectType == GameObjectType::Modifier && !isSpeedPortal) {
                    if (static_cast<EffectGameObject*>(obj)->m_isTouchTriggered) {
                        drawObjectBox(drawNode, obj->getObjectRect(), rotation, getFillColor(TRIGGER_COLOR), BORDER_WIDTH, TRIGGER_COLOR);
                    }
                    break;
                }
                drawObjectBox(drawNode, obj->getObjectRect(), rotation, getFillColor(OTHER_COLOR), BORDER_WIDTH, OTHER_COLOR);
                break;
            }
        }
    }

    // Player hitbox intentionally stays axis-aligned even during visual
    // spin animations - GD's actual player collision doesn't rotate with
    // the sprite, so rotating this box would misrepresent the real hitbox.
    void drawPlayerHitbox(CCDrawNode* drawNode, PlayerObject* player) {
        if (!player) return;
        drawRectOutline(drawNode, player->getObjectRect(), getFillColor(PLAYER_COLOR), BORDER_WIDTH, PLAYER_COLOR);
        drawRectOutline(drawNode, player->getObjectRect(0.3f, 0.3f), getFillColor(PLAYER_INNER_COLOR), BORDER_WIDTH, PLAYER_INNER_COLOR);
    }

    void renderTrailBuffer(CCDrawNode* drawNode, const RingBuffer<CCRect>& trail, ccColor4F color) {
        size_t count = trail.size();
        if (count == 0) return;

        bool fade = ShowHitboxes::alphaFadeEnabled();
        size_t index = 0;
        trail.for_each([&](const CCRect& rect) {
            float alpha = fade ? (static_cast<float>(index + 1) / static_cast<float>(count)) : 1.f;
            drawRectOutline(drawNode, rect, {0.f, 0.f, 0.f, 0.f}, BORDER_WIDTH, withAlpha(color, alpha));
            index++;
        });
    }

    // Iterates GD's own visible-object sections (the same spatial partition
    // the renderer itself uses) instead of scanning every object in the
    // level - correctly limits drawing to what's actually on screen.
    void redrawHitboxes(GJBaseGameLayer* layer, CCDrawNode* drawNode) {
        if (!drawNode) return;
        drawNode->clear();
        if (!ShowHitboxes::isEnabled()) return;

        // "Only On Death": stay hidden during normal play, only draw while
        // the post-death freeze window is active.
        if (ShowHitboxes::onlyOnDeathEnabled() && deathFreezeTimer <= 0.f) return;

        if (!layer->m_sections.empty()) {
            int rightBound = std::min(layer->m_rightSectionIndex, static_cast<int>(layer->m_sections.size()) - 1);
            for (int i = layer->m_leftSectionIndex; i <= rightBound; i++) {
                auto leftSection = layer->m_sections[i];
                if (!leftSection) continue;
                int topBound = std::min(layer->m_topSectionIndex, static_cast<int>(leftSection->size()) - 1);
                auto sizeRow = layer->m_sectionSizes[i];
                for (int j = layer->m_bottomSectionIndex; j <= topBound; j++) {
                    auto section = leftSection->at(j);
                    if (!section) continue;
                    int sectionSize = sizeRow->at(j);
                    auto data = section->data();
                    for (int k = 0; k < sectionSize; k++) {
                        drawObjectHitbox(layer, drawNode, data[k]);
                    }
                }
            }
        }

        drawPlayerHitbox(drawNode, layer->m_player1);
        if (layer->m_gameState.m_isDualMode) {
            drawPlayerHitbox(drawNode, layer->m_player2);
        }

        if (ShowHitboxes::trailEnabled()) {
            renderTrailBuffer(drawNode, playerTrail1, TRAIL_COLOR);
            renderTrailBuffer(drawNode, playerInnerTrail1, TRAIL_INNER_COLOR);

            if (layer->m_gameState.m_isDualMode) {
                renderTrailBuffer(drawNode, playerTrail2, TRAIL_COLOR);
                renderTrailBuffer(drawNode, playerInnerTrail2, TRAIL_INNER_COLOR);
            }
        }
    }

    void resetTrailState() {
        playerTrail1.clear();
        playerTrail2.clear();
        playerInnerTrail1.clear();
        playerInnerTrail2.clear();
        wasDead = false;
        deathFreezeTimer = 0.f;
    }
}

class $modify(MyShowHitboxesPlayLayer, PlayLayer) {
    struct Fields {
        CCDrawNode* drawNode = nullptr;
    };

    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        m_fields->drawNode = createHitboxDrawNode(this, m_debugDrawNode, "hitboxes"_spr);
    }

    void updateVisibility(float dt) {
        PlayLayer::updateVisibility(dt);
        redrawHitboxes(this, m_fields->drawNode);
    }

    void resetLevel() {
        resetTrailState();
        PlayLayer::resetLevel();
    }
};

class $modify(MyShowHitboxesEditorLayer, LevelEditorLayer) {
    struct Fields {
        CCDrawNode* drawNode = nullptr;
    };

    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;
        m_fields->drawNode = createHitboxDrawNode(this, m_debugDrawNode, "hitboxes-editor"_spr);
        return true;
    }

    void updateVisibility(float dt) {
        LevelEditorLayer::updateVisibility(dt);
        redrawHitboxes(this, m_fields->drawNode);
    }

    void onPlaytest() {
        resetTrailState();
        LevelEditorLayer::onPlaytest();
    }
};

class $modify(MyShowHitboxesTrailRecorder, GJBaseGameLayer) {
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);

        // Death-freeze timer countdown, independent of whether the trail
        // itself is enabled, so "Only On Death" works even with the trail off.
        bool diedNow = m_playerDied && !wasDead;
        wasDead = m_playerDied;
        if (diedNow) {
            deathFreezeTimer = DEATH_FREEZE_SECONDS;
        } else if (deathFreezeTimer > 0.f) {
            deathFreezeTimer -= dt;
            if (deathFreezeTimer < 0.f) deathFreezeTimer = 0.f;
        }

        if (!ShowHitboxes::trailEnabled()) return;

        size_t maxLength = static_cast<size_t>(ShowHitboxes::getTrailLength());
        if (lastTrailLength != maxLength) {
            playerTrail1.init(maxLength);
            playerTrail2.init(maxLength);
            playerInnerTrail1.init(maxLength);
            playerInnerTrail2.init(maxLength);
            lastTrailLength = maxLength;
        }

        if (m_playerDied) return;

        if (m_player1) {
            playerTrail1.push(m_player1->getObjectRect());
            playerInnerTrail1.push(m_player1->getObjectRect(0.3f, 0.3f));
        }
        if (m_gameState.m_isDualMode && m_player2) {
            playerTrail2.push(m_player2->getObjectRect());
            playerInnerTrail2.push(m_player2->getObjectRect(0.3f, 0.3f));
        }
    }
};

