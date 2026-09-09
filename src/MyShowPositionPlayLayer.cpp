#include <Geode/modify/PlayLayer.hpp>
#include "ShowPosition.hpp"

using namespace geode::prelude;

// Simple HUD overlay showing the player's live X/Y position. Uses
// postUpdate rather than update - that runs after physics resolves for the
// frame, so the number shown reflects the truly final position rather than
// a mid-step one.
class $modify(MyShowPositionPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* posLabel = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("X: 0\nY: 0", "bigFont.fnt");
        label->setScale(0.35f);
        label->setAnchorPoint({0.f, 1.f});
        label->setPosition({10.f, winSize.height - 10.f});
        label->setVisible(ShowPosition::isEnabled());
        this->addChild(label, 1000);
        m_fields->posLabel = label;

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_fields->posLabel || !m_player1) return;

        bool enabled = ShowPosition::isEnabled();
        m_fields->posLabel->setVisible(enabled);

        if (enabled) {
            auto pos = m_player1->getPosition();
            std::string text = "X: " + std::to_string(static_cast<int>(pos.x)) +
                                "\nY: " + std::to_string(static_cast<int>(pos.y));
            m_fields->posLabel->setString(text.c_str());
        }
    }
};
