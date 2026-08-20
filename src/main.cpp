#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Shared toggle state between the popup UI and the gameplay hook below.
static bool g_showHitboxes = false;

// =======================================================================
// ModMenuPopup
//
// Same button-opens-popup structure as before: one "Show Hitboxes" row.
// =======================================================================
class ModMenuPopup : public geode::Popup {
protected:
    static constexpr float POPUP_WIDTH = 240.f;
    static constexpr float POPUP_HEIGHT = 120.f;

    void onToggleHitboxes(CCObject* sender) {
        g_showHitboxes = static_cast<CCMenuItemToggler*>(sender)->isToggled();
    }

    bool init() {
        if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT))
            return false;

        this->setTitle("Enz7y's Mod Menu");

        auto row = CCMenu::create();
        row->setPosition({ 0.f, 0.f });
        row->ignoreAnchorPointForPosition(false);
        m_mainLayer->addChild(row);

        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

        auto toggle = CCMenuItemToggler::create(
            offSpr, onSpr, this, menu_selector(ModMenuPopup::onToggleHitboxes)
        );
        toggle->setScale(0.8f);
        toggle->setPosition({ 405.f, 205.f });
        row->addChild(toggle);

        auto text = CCLabelBMFont::create("Show Hitboxes", "bigFont.fnt");
        text->setScale(0.45f);
        text->setAnchorPoint({ 0.f, 0.5f });
        text->setPosition({ 425.f, 205.f });
        row->addChild(text);

        return true;
    }

public:
    static ModMenuPopup* create() {
        auto ret = new ModMenuPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

// =======================================================================
// Hook the main menu to add the button that opens the popup (unchanged).
// =======================================================================
class $modify(ModMenuHook, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto buttonSprite = ButtonSprite::create("Menu");
        buttonSprite->setScale(0.6f);

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite, this, menu_selector(ModMenuHook::onOpenModMenu)
        );

        auto menu = CCMenu::create();
        menu->addChild(button);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        menu->setPosition({ winSize.width - 40.f, winSize.height - 30.f });

        this->addChild(menu, 100);

        return true;
    }

    void onOpenModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};

// =======================================================================
// HitboxPlayLayerHook
//
// A self-contained hitbox drawer: every frame, while the toggle is on,
// it draws a box around the player and around nearby level objects
// using a CCDrawNode.
//
// How it works:
//   - Each object's box is its own sprite bounds (getContentSize() x
//     getScale()), NOT GD's internal collision polygon. For most blocks
//     this lines up closely; for objects with a smaller real hitbox
//     than their sprite (spikes, some slopes) the box will be a bit
//     more generous than the true hitbox. It's a visual approximation,
//     not a readout of GD's actual collision data.
//   - Only objects within ~700 units of the player are drawn, to avoid
//     drawing hundreds/thousands of boxes on long levels every frame.
//   - The draw node is parented alongside the player (rather than
//     directly under PlayLayer) so it scrolls correctly with the
//     camera.
// =======================================================================
class $modify(HitboxPlayLayerHook, PlayLayer) {
    struct Fields {
        CCDrawNode* m_hitboxNode = nullptr;
    };

    static constexpr float DRAW_RANGE = 700.f;

    void drawHitboxFor(CCNode* node, ccColor4F const& color) {
        if (!node) return;

        auto size = node->getContentSize();
        auto pos = node->getPosition();
        float halfW = (size.width * node->getScaleX()) / 2.f;
        float halfH = (size.height * node->getScaleY()) / 2.f;

        m_fields->m_hitboxNode->drawRect(
            { pos.x - halfW, pos.y - halfH },
            { pos.x + halfW, pos.y + halfH },
            ccc4f(0.f, 0.f, 0.f, 0.f),
            1.5f,
            color
        );
    }

    void update(float dt) {
        PlayLayer::update(dt);

        // Lazily create the draw node once we have a player to anchor
        // it to — parenting it alongside the player keeps it in the
        // same scrolling coordinate space as the level objects.
        if (!m_fields->m_hitboxNode && this->m_player1 && this->m_player1->getParent()) {
            m_fields->m_hitboxNode = CCDrawNode::create();
            this->m_player1->getParent()->addChild(m_fields->m_hitboxNode, 10000);
        }

        auto drawNode = m_fields->m_hitboxNode;
        if (!drawNode) return;

        drawNode->clear();
        if (!g_showHitboxes) return;

        auto playerPos = this->m_player1 ? this->m_player1->getPosition() : CCPointZero;

        if (this->m_objects) {
            CCArrayExt<GameObject*> objects = this->m_objects;
            for (auto* obj : objects) {
                if (!obj) continue;
                auto d = obj->getPosition() - playerPos;
                if (std::fabs(d.x) > DRAW_RANGE || std::fabs(d.y) > DRAW_RANGE) continue;
                drawHitboxFor(obj, ccc4f(1.f, 0.f, 0.f, 1.f));
            }
        }

        drawHitboxFor(this->m_player1, ccc4f(0.f, 1.f, 0.f, 1.f));
        if (this->m_player2 && this->m_player2 != this->m_player1) {
            drawHitboxFor(this->m_player2, ccc4f(0.f, 1.f, 0.f, 1.f));
        }
    }
};