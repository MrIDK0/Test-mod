#include "ModMenuPopup.hpp"
#include "SpeedHack.hpp"

bool ModMenuPopup::init() {
    if (!Popup::init(240.f, 160.f))
        return false;

    this->setTitle("Mod Menu");

    // --- Feature 1: Speed Hack ---
    auto speedToggle = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenuPopup::onSpeedHackToggle), 0.7f
    );
    speedToggle->setPosition({35, 110});
    speedToggle->toggle(Mod::get()->getSavedValue<bool>("speedhack-enabled", false));
    m_buttonMenu->addChild(speedToggle);

    auto speedLabel = CCLabelBMFont::create("Speed Hack", "bigFont.fnt");
    speedLabel->setScale(0.4f);
    speedLabel->setAnchorPoint({0.f, 0.5f});
    speedLabel->setPosition({60, 110});
    m_mainLayer->addChild(speedLabel);

    // Text input for the custom speed value, e.g. "1.5", "2", "0.5"
    auto speedInput = TextInput::create(80.f, "1.0", "bigFont.fnt");
    speedInput->setFilter("0123456789.");
    speedInput->setMaxCharCount(6);
    speedInput->setString(Mod::get()->getSavedValue<std::string>("speedhack-value", "1.0"));
    speedInput->setPosition({180, 110});
    speedInput->setCallback([](std::string const& text) {
        Mod::get()->setSavedValue("speedhack-value", text);
        SpeedHack::apply();
    });
    m_mainLayer->addChild(speedInput);

    // --- Feature 2: Show Hitboxes ---
    auto hitboxToggle = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenuPopup::onHitboxToggle), 0.7f
    );
    hitboxToggle->setPosition({35, 60});
    hitboxToggle->toggle(Mod::get()->getSavedValue<bool>("show-hitboxes", false));
    m_buttonMenu->addChild(hitboxToggle);

    auto hitboxLabel = CCLabelBMFont::create("Show Hitboxes", "bigFont.fnt");
    hitboxLabel->setScale(0.4f);
    hitboxLabel->setAnchorPoint({0.f, 0.5f});
    hitboxLabel->setPosition({60, 60});
    m_mainLayer->addChild(hitboxLabel);

    return true;
}

void ModMenuPopup::onSpeedHackToggle(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("speedhack-enabled", state);
    SpeedHack::apply();
}

void ModMenuPopup::onHitboxToggle(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("show-hitboxes", state);
}

ModMenuPopup* ModMenuPopup::create() {
    auto ret = new ModMenuPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
