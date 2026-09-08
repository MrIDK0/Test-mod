#include <Geode/modify/PlayLayer.hpp>
#include "AutoCheckpoint.hpp"

using namespace geode::prelude;

// Instead of guessing at internal functions, this finds the REAL checkpoint
// button in Practice Mode's UI ("add-checkpoint-button") and activates it
// exactly as if it were tapped - whatever logic is really behind that
// button just runs, since we're not reimplementing it ourselves.
class $modify(MyAutoCheckpointPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (AutoCheckpoint::isEnabled() && m_isPracticeMode) {
            auto btn = this->getChildByIDRecursive("add-checkpoint-button");
            if (auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
                menuItem->activate();
            }
        }
    }
};
