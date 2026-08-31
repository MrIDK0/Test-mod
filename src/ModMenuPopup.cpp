#include "ModMenuPopup.hpp"

bool ModMenuPopup::setup() {
    this->setTitle("Mod Menu");

    auto toggle1 = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenuPopup::onToggle1), 0.7f
    );
    toggle1->setPosition({80, 80});
    toggle1->toggle(Mod::get()->getSavedValue<bool>("feature-1", false));
    m_buttonMenu->addChild(toggle1);

    auto label1 = CCLabelBMFont::create("Feature 1", "bigFont.fnt");
    label1->setScale(0.4f);
    label1->setPosition({150, 80});
    m_mainLayer->addChild(label1);

    auto toggle2 = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenuPopup::onToggle2), 0.7f
    );
    toggle2->setPosition({80, 50});
    toggle2->toggle(Mod::get()->getSavedValue<bool>("feature-2", false));
    m_buttonMenu->addChild(toggle2);

    auto label2 = CCLabelBMFont::create("Feature 2", "bigFont.fnt");
    label2->setScale(0.4f);
    label2->setPosition({150, 50});
    m_mainLayer->addChild(label2);

    return true;
}

void ModMenuPopup::onToggle1(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("feature-1", state);
}

void ModMenuPopup::onToggle2(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("feature-2", state);
}

ModMenuPopup* ModMenuPopup::create() {
    auto ret = new ModMenuPopup();
    if (ret->initAnchored(240.f, 160.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
