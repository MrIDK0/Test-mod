#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Shared toggle state between the popup UI and the gameplay hook below.
static bool g_showHitboxes = false;

// =======================================================================
// ModMenuPopup
//
// Same button-opens-popup structure as before, trimmed down to a
// single row: a "Show Hitboxes" toggle.
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
        toggle->setPosition({ 90.f, 60.f });
        row->addChild(toggle);

        auto text = CCLabelBMFont::create("Show Hitboxes", "bigFont.fnt");
        text->setScale(0.45f);
        text->setAnchorPoint({ 0.f, 0.5f });
        text->setPosition({ 115.f, 60.f });
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
// Hook the main menu to add the button that opens the popup (unchanged
// from the earlier version).
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
// PlayLayer hook: while the toggle is on, forces the game's own
// hitbox-drawing pass (GJBaseGameLayer::updateDebugDraw) to run every
// frame. This reuses GD's real collision boxes instead of approximating
// them by hand.
//
// This is the one line most likely to need adjusting: updateDebugDraw
// is an internal engine function, not a documented/stable API. If it
// doesn't compile against your SDK version, open the DevTools mod
// in-game (F11) or check github.com/geode-sdk/bindings for the current
// signature.
// =======================================================================
class $modify(HitboxPlayLayerHook, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (g_showHitboxes) {
            this->updateDebugDraw();
        }
    }
};
