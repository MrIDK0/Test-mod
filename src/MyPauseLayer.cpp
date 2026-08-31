#include <Geode/modify/PauseLayer.hpp>
#include "ModMenuPopup.hpp"

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setID("mod-menu-side-menu"_spr);
        menu->setPosition({0, 0});
        this->addChild(menu, 100);

        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Mod"),
            this,
            menu_selector(MyPauseLayer::onOpenModMenu)
        );
        btn->setID("mod-menu-button"_spr);

        // Bottom-right corner, below the side icon column.
        // Open DevTools (F11 in-game) to read the exact position you want
        // and adjust these two numbers to match.
        btn->setPosition({winSize.width - 45.f, 55.f});

        menu->addChild(btn);
    }

    void onOpenModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};
