#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "RingBuffer.hpp"
#include "MacroBuff.hpp"

using namespace geode::prelude;

// Placeholder default for the second object type placed alongside the
// spike at every trail point - change to whatever object ID you actually
// want (verify it in the editor's object info panel).
namespace MacroBuffConfig {
    constexpr int SECOND_OBJECT_ID = 1817;
}

// Editor-only helper: while playtesting in the level editor, this tracks a
// rolling window of the player's recent hitbox positions and automatically
// places a spike plus a second object tracing the jump arc, then removes
// any of either that the player's own trail passes back through as you
// refine the jump on retest.
//
// Experimental - works best on pure jumps in Cube/Robot. Pads and gravity
// shifts break the trail's assumptions.
class $modify(MacroBuffGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool cached = false;
        CCPoint spikeOffset;
        CCSize spikeSize;
        CCPoint secondOffset;
        CCSize secondSize;

        RingBuffer<CCRect> player_trail{480};
        RingBuffer<GameObject*> spikes{800};
        RingBuffer<GameObject*> secondObjects{800};
        int frame = 0;
    };

    void cacheObjects() {
        if (m_fields->cached) return;
        auto lel = LevelEditorLayer::get();
        if (!lel) return;

        auto temp = lel->createObject(8, CCPoint(0.f, 0.f), true);
        if (!temp) return;
        CCRect r = temp->getObjectRect();
        m_fields->spikeOffset = temp->getPosition() - r.origin;
        m_fields->spikeSize = r.size;
        lel->removeObject(temp, true);

        auto temp2 = lel->createObject(MacroBuffConfig::SECOND_OBJECT_ID, CCPoint(0.f, 0.f), true);
        if (temp2) {
            CCRect r2 = temp2->getObjectRect();
            m_fields->secondOffset = temp2->getPosition() - r2.origin;
            m_fields->secondSize = r2.size;
            lel->removeObject(temp2, true);
        }

        m_fields->cached = true;
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);

        if (!MacroBuff::isEnabled()) return;

        auto lel = LevelEditorLayer::get();
        if (!lel || !m_player1) return;

        cacheObjects();
        if (!m_fields->cached) return;

        CCRect playerRect = m_player1->getObjectRect();
        m_fields->player_trail.push(playerRect);

        auto checkIntersects = [](const CCRect& r1, const CCRect& r2) {
            return !(r1.origin.x + r1.size.width <= r2.origin.x ||
                    r2.origin.x + r2.size.width  <= r1.origin.x ||
                    r1.origin.y + r1.size.height <= r2.origin.y ||
                    r2.origin.y + r2.size.height <= r1.origin.y);
        };

        auto cleanupIfTouched = [&](RingBuffer<GameObject*>& buffer) {
            buffer.for_each([&](GameObject*& obj) {
                if (!obj) return;

                CCRect currentRect = obj->getObjectRect();
                bool shouldRemove = false;

                m_fields->player_trail.for_each([&](const auto& trailRect) {
                    if (!shouldRemove && checkIntersects(currentRect, trailRect)) {
                        shouldRemove = true;
                    }
                });

                if (shouldRemove) {
                    lel->removeObject(obj, true);
                    obj = nullptr;
                }
            });
        };

        cleanupIfTouched(m_fields->spikes);
        cleanupIfTouched(m_fields->secondObjects);

        if (m_player1->m_isOnGround) return;

        m_fields->frame++;
        if (m_fields->frame < 5) {
            return;
        }
        m_fields->frame = 0;

        float vy = m_player1->m_yVelocity;

        auto spawnSpikeAt = [&](float originX, float originY) {
            CCPoint targetOrigin(originX, originY);

            CCPoint finalPos = targetOrigin + m_fields->spikeOffset;
            auto obj = lel->createObject(8, finalPos, true);
            if (obj) {
                m_fields->spikes.push(obj);
            }

            CCPoint secondFinalPos = targetOrigin + m_fields->secondOffset;
            auto obj2 = lel->createObject(MacroBuffConfig::SECOND_OBJECT_ID, secondFinalPos, true);
            if (obj2) {
                m_fields->secondObjects.push(obj2);
            }
        };

        if (vy >= 0.f) {
            spawnSpikeAt(
                playerRect.origin.x - m_fields->spikeSize.width - 0.0001f,
                playerRect.origin.y + playerRect.size.height + 0.0001f
            );
        } else {
            spawnSpikeAt(
                playerRect.origin.x + playerRect.size.width + 0.0001f,
                playerRect.origin.y + playerRect.size.height + 0.0001f
            );
        }

        if (std::abs(vy) >= 5.0f) {
            if (vy >= 0.f) {
                spawnSpikeAt(
                    playerRect.origin.x + playerRect.size.width + 0.0001f,
                    playerRect.origin.y - m_fields->spikeSize.height - 0.0001f
                );
            } else {
                spawnSpikeAt(
                    playerRect.origin.x - m_fields->spikeSize.width - 0.0001f,
                    playerRect.origin.y - m_fields->spikeSize.height - 0.0001f
                );
            }
        }
    }
};

class $modify(MacroBuffEditorLayer, LevelEditorLayer) {
    void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        if (auto gjbgl = static_cast<MacroBuffGJBaseGameLayer*>(GJBaseGameLayer::get())) {
            gjbgl->m_fields->player_trail.clear();
            gjbgl->m_fields->spikes.clear();
            gjbgl->m_fields->secondObjects.clear();
            gjbgl->m_fields->frame = 0;
        }
    }
};
