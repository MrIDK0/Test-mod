#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

// Practice Mode already lets you place a checkpoint manually. This does
// the same thing automatically, every single frame, so wherever you die
// you respawn practically exactly where you were - the densest possible
// safety net. Only does anything in Practice Mode, since normal mode
// doesn't support checkpoints at all.
class $modify(MyAutoCheckpointPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* debugLabel = nullptr;
        int frameCounter = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // TEMPORARY debug label - top-center, large, so it's impossible to
        // miss or confuse with anything else on screen. Includes a counter
        // that increments every frame, so even if the rest of the text
        // looks unchanged, the number climbing proves update() is running.
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("CP DEBUG STARTING", "bigFont.fnt");
        label->setScale(0.6f);
        label->setPosition({winSize.width / 2.f, winSize.height - 30.f});
        label->setColor({255, 255, 0});
        this->addChild(label, 5000);
        m_fields->debugLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        m_fields->frameCounter++;

        bool enabled = AutoCheckpoint::isEnabled();

        if (enabled && m_isPracticeMode) {
            auto checkpoint = this->createCheckpoint();
            if (checkpoint) {
                this->storeCheckpoint(checkpoint);
            }
        }

        if (m_fields->debugLabel) {
            std::string text =
                "F:" + std::to_string(m_fields->frameCounter) +
                " EN:" + std::string(enabled ? "Y" : "N") +
                " PRAC:" + std::string(m_isPracticeMode ? "Y" : "N");
            m_fields->debugLabel->setString(text.c_str());
        }
    }
};
