#include <Geode/modify/GJBaseGameLayer.hpp>
#include "AutoClickPad.hpp"
#include <vector>

using namespace geode::prelude;

// Hooked on GJBaseGameLayer (not PlayLayer) so this also works in the
// editor's test mode, which inherits from the same base.
class $modify(MyAutoClickPadLayer, GJBaseGameLayer) {
    struct Fields {
        std::vector<GameObject*> targetPads;
        bool cached = false;
        bool wasNear = false;
        int clickFramesRemaining = 0;
    };

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        // Process active click duration frame-by-frame
        if (m_fields->clickFramesRemaining > 0) {
            m_fields->clickFramesRemaining--;
            
            // Once the frame counter reaches 0, send the release signal
            if (m_fields->clickFramesRemaining == 0) {
                GJBaseGameLayer::handleButton(false, AutoClickPadConfig::CLICK_BUTTON, true);
            }
        }

        if (!AutoClickPad::isEnabled()) {
            m_fields->wasNear = false;
            return;
        }

        // Build the list of matching pads once instead of scanning every
        // object in the level on every single frame.
        if (!m_fields->cached) {
            m_fields->targetPads.clear();
            if (m_objects) {
                for (int i = 0; i < m_objects->count(); i++) {
                    auto obj = static_cast<GameObject*>(m_objects->objectAtIndex(i));
                    if (obj && obj->m_objectID == AutoClickPadConfig::TARGET_OBJECT_ID) {
                        m_fields->targetPads.push_back(obj);
                    }
                }
            }
            m_fields->cached = true;
        }

        bool nowNear = false;
        if (m_player1) {
            auto playerPos = m_player1->getPosition();
            for (auto* pad : m_fields->targetPads) {
                if (!pad) continue;
                float dist = ccpDistance(playerPos, pad->getPosition());
                if (dist <= AutoClickPadConfig::TRIGGER_RADIUS) {
                    nowNear = true;
                    break;
                }
            }
        }

        // Rising edge only - triggers the initial click and sets the frame timer
        if (nowNear && !m_fields->wasNear) {
            GJBaseGameLayer::handleButton(true, AutoClickPadConfig::CLICK_BUTTON, true);
            m_fields->clickFramesRemaining = AutoClickPadConfig::CLICK_DURATION_FRAMES;
        }

        m_fields->wasNear = nowNear;
    }
};
