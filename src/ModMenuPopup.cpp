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

    // --- Feature 2 (placeholder toggle, unchanged) ---
    auto toggle2 = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenuPopup::onToggle2), 0.7f
    );
    toggle2->setPosition({35, 60});
    toggle2->toggle(Mod::get()->getSavedValue<bool>("feature-2", false));
    m_buttonMenu->addChild(toggle2);

    auto label2 = CCLabelBMFont::create("Feature 2", "bigFont.fnt");
    label2->setScale(0.4f);
    label2->setAnchorPoint({0.f, 0.5f});
    label2->setPosition({60, 60});
    m_mainLayer->addChild(label2);

    return true;
}

void ModMenuPopup::onSpeedHackToggle(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("speedhack-enabled", state);
    SpeedHack::apply();
}

void ModMenuPopup::onToggle2(CCObject* sender) {
    bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("feature-2", state);
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
