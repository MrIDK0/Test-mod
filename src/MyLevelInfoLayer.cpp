#include <Geode/modify/LevelInfoLayer.hpp>
#include "ModMenuPopup.hpp"

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Its own menu so it doesn't fight over layout with existing buttons
        auto menu = CCMenu::create();
        menu->setID("mod-menu-side-menu"_spr);
        menu->setPosition({0, 0});
        this->addChild(menu, 100);

        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Mod"),
            this,
            menu_selector(MyLevelInfoLayer::onOpenModMenu)
        );
        btn->setID("mod-menu-button"_spr);

        // Positioned in the bottom-right corner, below the side icon column.
        // Tweak these offsets in DevTools (F11) until it lines up exactly
        // where you want against the other side buttons.
        btn->setPosition({winSize.width - 45.f, 55.f});

        menu->addChild(btn);

        return true;
    }

    void onOpenModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};
