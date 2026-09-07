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
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // TEMPORARY debug label - shows the actual size of the checkpoint
        // array live, so we can tell for certain whether anything is being
        // added at all, independent of whatever's visible on screen.
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("cp debug", "chatFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint({1.f, 1.f});
        label->setPosition({winSize.width - 5.f, winSize.height - 5.f});
        this->addChild(label, 2000);
        m_fields->debugLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        bool enabled = AutoCheckpoint::isEnabled();

        if (enabled && m_isPracticeMode) {
            m_tryPlaceCheckpoint = true;
        }

        if (m_fields->debugLabel) {
            int count = m_checkpointArray ? m_checkpointArray->count() : -1;
            std::string text =
                "CP count: " + std::to_string(count) +
                " | flag:" + std::string(m_tryPlaceCheckpoint ? "1" : "0");
            m_fields->debugLabel->setString(text.c_str());
        }
    }
};
