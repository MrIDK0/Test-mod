#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "ModMenuPopup.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = this->getChildByID("bottom-menu");
        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Menu"),
            this,
            menu_selector(MyMenuLayer::onOpenModMenu)
        );
        btn->setID("mod-menu-button"_spr);
        menu->addChild(btn);
        menu->updateLayout();

        return true;
    }

    void onOpenModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};
