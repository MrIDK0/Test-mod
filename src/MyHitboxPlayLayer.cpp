#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyHitboxPlayLayer, PlayLayer) {
    struct Fields {
        CCDrawNode* hitboxNode = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto node = CCDrawNode::create();
        // High z so it draws on top of the level, not behind objects
        this->addChild(node, 1000);
        m_fields->hitboxNode = node;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto node = m_fields->hitboxNode;
        if (!node) return;

        node->clear();

        if (!Mod::get()->getSavedValue<bool>("show-hitboxes", false)) return;

        auto drawHitbox = [&](PlayerObject* player, ccColor4F color) {
            if (!player) return;
            auto rect = player->getObjectRect();
            node->drawRect(
                rect.origin,
                {rect.origin.x + rect.size.width, rect.origin.y + rect.size.height},
                {0.f, 0.f, 0.f, 0.f},   // transparent fill, outline only
                2.f,
                color
            );
        };

        drawHitbox(m_player1, {1.f, 0.f, 0.f, 1.f}); // red
        if (m_player2 && m_player2->isVisible()) {
            drawHitbox(m_player2, {0.f, 0.6f, 1.f, 1.f}); // blue
        }
    }
};
