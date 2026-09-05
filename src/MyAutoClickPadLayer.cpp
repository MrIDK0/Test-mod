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
        bool wasTouching = false;
        bool releaseNextFrame = false;
    };

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        // Release a pending click exactly 1 frame after it was pressed,
        // regardless of current toggle state - avoids ever leaving the
        // button stuck down if the toggle gets flipped off mid-press.
        if (m_fields->releaseNextFrame) {
            GJBaseGameLayer::handleButton(false, AutoClickPadConfig::CLICK_BUTTON, true);
            m_fields->releaseNextFrame = false;
        }

        if (!AutoClickPad::isEnabled()) {
            m_fields->wasTouching = false;
            return;
        }

        // Build the list of matching pads once instead of scanning every
        // object in the level on every single frame. Any object whose ID
        // is in AutoClickPadConfig::TARGET_OBJECT_IDS gets included, so
        // this works for however many pad types are configured there.
        if (!m_fields->cached) {
            m_fields->targetPads.clear();
            if (m_objects) {
                for (int i = 0; i < m_objects->count(); i++) {
                    auto obj = static_cast<GameObject*>(m_objects->objectAtIndex(i));
                    if (obj && AutoClickPadConfig::isTargetPad(obj->m_objectID)) {
                        m_fields->targetPads.push_back(obj);
                    }
                }
            }
            m_fields->cached = true;
        }

        // Real touch detection: intersect the player's actual hitbox
        // against each pad's actual hitbox, rather than guessing from
        // distance between center points.
        bool nowTouching = false;
        if (m_player1) {
            auto playerRect = m_player1->getObjectRect();
            for (auto* pad : m_fields->targetPads) {
                if (!pad) continue;
                if (playerRect.intersectsRect(pad->getObjectRect())) {
                    nowTouching = true;
                    break;
                }
            }
        }

        // Rising edge only - clicks once per touch, not every frame
        // you happen to still be overlapping the pad.
        if (nowTouching && !m_fields->wasTouching) {
            GJBaseGameLayer::handleButton(true, AutoClickPadConfig::CLICK_BUTTON, true);
            m_fields->releaseNextFrame = true;
        }

        m_fields->wasTouching = nowTouching;
    }
};
