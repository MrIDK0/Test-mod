#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "ShowHitboxes.hpp"

using namespace geode::prelude;

namespace {
    constexpr ccColor4F DANGER_COLOR       = {1.f, 0.f, 0.f, 1.f};
    constexpr ccColor4F SOLID_COLOR        = {0.f, 0.25f, 1.f, 1.f};
    constexpr ccColor4F PASSABLE_COLOR     = {0.f, 1.f, 1.f, 1.f};
    constexpr ccColor4F TRIGGER_COLOR      = {1.f, 0.f, 0.9f, 1.f};
    constexpr ccColor4F OTHER_COLOR        = {0.f, 1.f, 0.f, 1.f};
    constexpr ccColor4F PLAYER_COLOR       = {1.f, 0.f, 0.f, 1.f};
    constexpr ccColor4F PLAYER_INNER_COLOR = {0.f, 0.25f, 1.f, 1.f};
    constexpr float BORDER_WIDTH = 0.25f;
    constexpr float FILL_ALPHA   = 0.2f;

    ccColor4F withAlpha(ccColor4F c, float a) {
        return {c.r, c.g, c.b, a};
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

    void drawRectOutline(CCDrawNode* node, const CCRect& rect, ccColor4F fill, float border, ccColor4F borderColor) {
        CCPoint verts[4] = {
            {rect.getMinX(), rect.getMinY()},
            {rect.getMinX(), rect.getMaxY()},
            {rect.getMaxX(), rect.getMaxY()},
            {rect.getMaxX(), rect.getMinY()}
        };
        node->drawPolygon(verts, 4, fill, border, borderColor);
    }

    void drawObjectHitbox(GJBaseGameLayer* layer, CCDrawNode* drawNode, GameObject* obj) {
        if (!obj) return;
        if (obj->m_objectType == GameObjectType::Decoration || !obj->m_isActivated || obj->m_isGroupDisabled)
            return;
        if (obj == layer->m_player1 || obj == layer->m_player2) return;

        switch (obj->m_objectType) {
            case GameObjectType::Solid: {
                bool passable = obj->m_isPassable;
                auto color = passable ? PASSABLE_COLOR : SOLID_COLOR;
                drawRectOutline(drawNode, obj->getObjectRect(), withAlpha(color, FILL_ALPHA), BORDER_WIDTH, color);
                break;
            }
            case GameObjectType::Hazard:
            case GameObjectType::AnimatedHazard: {
                if (obj == layer->m_anticheatSpike) break;
                float radius = std::max(obj->m_scaleX, obj->m_scaleY) * obj->m_objectRadius;
                if (radius > 0.f) {
                    drawNode->drawCircle(obj->getPosition(), radius, withAlpha(DANGER_COLOR, FILL_ALPHA), BORDER_WIDTH, DANGER_COLOR, 16);
                } else {
                    drawRectOutline(drawNode, obj->getObjectRect(), withAlpha(DANGER_COLOR, FILL_ALPHA), BORDER_WIDTH, DANGER_COLOR);
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
                        drawRectOutline(drawNode, obj->getObjectRect(), withAlpha(TRIGGER_COLOR, FILL_ALPHA), BORDER_WIDTH, TRIGGER_COLOR);
                    }
                    break;
                }
                drawRectOutline(drawNode, obj->getObjectRect(), withAlpha(OTHER_COLOR, FILL_ALPHA), BORDER_WIDTH, OTHER_COLOR);
                break;
            }
        }
    }

    void drawPlayerHitbox(CCDrawNode* drawNode, PlayerObject* player) {
        if (!player) return;
        drawRectOutline(drawNode, player->getObjectRect(), withAlpha(PLAYER_COLOR, FILL_ALPHA), BORDER_WIDTH, PLAYER_COLOR);
        drawRectOutline(drawNode, player->getObjectRect(0.3f, 0.3f), withAlpha(PLAYER_INNER_COLOR, FILL_ALPHA), BORDER_WIDTH, PLAYER_INNER_COLOR);
    }

    // Iterates GD's own visible-object sections (the same spatial partition
    // the renderer itself uses) instead of scanning every object in the
    // level - correctly limits drawing to what's actually on screen.
    void redrawHitboxes(GJBaseGameLayer* layer, CCDrawNode* drawNode) {
        if (!drawNode) return;
        drawNode->clear();
        if (!ShowHitboxes::isEnabled()) return;

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
};
