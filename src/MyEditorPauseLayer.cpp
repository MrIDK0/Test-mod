#include <Geode/modify/EditorPauseLayer.hpp>
#include "ModMenuPopup.hpp"

using namespace geode::prelude;

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
    void customSetup() {
        EditorPauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setID("mod-menu-editor-side-menu"_spr);
        menu->setPosition({0, 0});
        this->addChild(menu, 100);

        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Mod"),
            this,
            menu_selector(MyEditorPauseLayer::onOpenModMenu)
        );
        btn->setID("mod-menu-editor-button"_spr);

        // Same corner spot as the gameplay pause menu button - nudge via
        // DevTools if you want it somewhere else in the editor specifically.
        btn->setPosition({winSize.width - 45.f, 55.f});

        menu->addChild(btn);
    }

    void onOpenModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};
