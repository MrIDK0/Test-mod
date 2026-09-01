#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyHitboxPlayLayer, PlayLayer) {
    struct Fields {
        CCDrawNode* hitboxNode = nullptr;
        CCLabelBMFont* debugLabel = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // TEMPORARY debug label - shows live status in the top-left corner
        // so we can see what's happening without needing DevTools/a mouse.
        // Safe to remove once hitboxes are confirmed working.
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("hitbox debug", "chatFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint({0.f, 1.f});
        label->setPosition({5.f, winSize.height - 5.f});
        this->addChild(label, 2000);
        m_fields->debugLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        // Create the draw node the first moment the player actually exists,
        // whenever that happens to be - avoids relying on init() timing.
        if (!m_fields->hitboxNode && m_player1 && m_player1->getParent()) {
            auto node = CCDrawNode::create();
            m_player1->getParent()->addChild(node, 1000);
            m_fields->hitboxNode = node;
        }

        if (m_fields->debugLabel) {
            std::string text =
                "P1:" + std::string(m_player1 ? "yes" : "NO") +
                " Node:" + std::string(m_fields->hitboxNode ? "yes" : "NO") +
                " Toggle:" + std::string(Mod::get()->getSavedValue<bool>("show-hitboxes", false) ? "ON" : "off");
            m_fields->debugLabel->setString(text.c_str());
        }

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
