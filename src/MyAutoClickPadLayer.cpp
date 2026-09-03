#include <Geode/modify/GJBaseGameLayer.hpp>
#include "AutoClickPad.hpp"
#include <vector>

using namespace geode::prelude;

class $modify(MyAutoClickPadLayer, GJBaseGameLayer) {
    struct Fields {
        std::vector<GameObject*> targetPads;
        bool cached = false;
        bool wasNear = false;
        int activeClickFrames = 0;
    };

    void processCommands() {
        // Run standard game commands first
        GJBaseGameLayer::processCommands();

        if (!AutoClickPad::isEnabled()) {
            m_fields->wasNear = false;
            m_fields->activeClickFrames = 0;
            return;
        }

        // Cache target objects once on level load
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

        // Check player proximity
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

        // Trigger new click sequence on rising edge
        if (nowNear && !m_fields->wasNear) {
            m_fields->activeClickFrames = AutoClickPadConfig::CLICK_DURATION_FRAMES;
        }

        // Handle exact frame duration
        if (m_fields->activeClickFrames > 0) {
            // Send push signal
            GJBaseGameLayer::handleButton(true, AutoClickPadConfig::CLICK_BUTTON, true);
            m_fields->activeClickFrames--;

            // Release precisely on the final frame
            if (m_fields->activeClickFrames == 0) {
                GJBaseGameLayer::handleButton(false, AutoClickPadConfig::CLICK_BUTTON, true);
            }
        }

        m_fields->wasNear = nowNear;
    }
};
