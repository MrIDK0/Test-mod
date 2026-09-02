#include "ModMenuPopup.hpp"
#include "SpeedHack.hpp"

namespace {
    constexpr float POPUP_WIDTH   = 420.f;
    constexpr float POPUP_HEIGHT  = 260.f;
    constexpr float SIDEBAR_WIDTH = 90.f;

    constexpr float GRID_TOP_Y  = 170.f;
    constexpr float ROW_HEIGHT  = 34.f;
    constexpr float COL_X[2]    = {40.f, 200.f};

    constexpr const char* TAB_NAMES[ModMenuPopup::TAB_COUNT] = {
        "Core", "Cosmetic", "Level", "Creator"
    };
    constexpr const char* TAB_KEYS[ModMenuPopup::TAB_COUNT] = {
        "core", "cosmetic", "level", "creator"
    };
}

bool ModMenuPopup::init() {
    if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT)) return false;
    this->setTitle("Mod Menu");

    // --- Sidebar tabs ---
    auto sidebarMenu = CCMenu::create();
    sidebarMenu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(sidebarMenu);

    float sidebarTop = 220.f;
    float tabHeight = sidebarTop / TAB_COUNT;
    for (int i = 0; i < TAB_COUNT; i++) {
        float yPos = sidebarTop - tabHeight * i - tabHeight / 2.f;
        addTabButton(sidebarMenu, i, TAB_NAMES[i], yPos);
    }

    // --- One page + one menu per tab. Only the active tab's page is
    // visible AND its menu enabled, so hidden tabs can't intercept touches
    // meant for the visible one. ---
    for (int i = 0; i < TAB_COUNT; i++) {
        auto page = CCNode::create();
        page->setPosition({SIDEBAR_WIDTH, 0.f});
        page->setVisible(i == 0);
        m_mainLayer->addChild(page);
        m_pages[i] = page;

        auto pageMenu = CCMenu::create();
        pageMenu->setPosition({0.f, 0.f});
        pageMenu->setEnabled(i == 0);
        page->addChild(pageMenu);
        m_pageMenus[i] = pageMenu;
    }

    // --- Core tab: Speed Hack is the one real feature so far ---
    {
        auto page = m_pages[0];
        auto menu = m_pageMenus[0];

        auto speedToggle = CCMenuItemExt::createTogglerWithStandardSprites(
            0.6f,
            [](CCMenuItemToggler* toggle) {
                bool state = !toggle->isToggled();
                Mod::get()->setSavedValue("speedhack-enabled", state);
                SpeedHack::apply();
            }
        );
        speedToggle->toggle(Mod::get()->getSavedValue<bool>("speedhack-enabled", false));
        speedToggle->setPosition({COL_X[0], GRID_TOP_Y});
        menu->addChild(speedToggle);

        auto speedLabel = CCLabelBMFont::create("Speed Hack", "bigFont.fnt");
        speedLabel->setScale(0.35f);
        speedLabel->setAnchorPoint({0.f, 0.5f});
        speedLabel->setPosition({COL_X[0] + 20.f, GRID_TOP_Y});
        page->addChild(speedLabel);

        auto speedInput = TextInput::create(70.f, "1.0", "bigFont.fnt");
        speedInput->setFilter("0123456789.");
        speedInput->setMaxCharCount(6);
        speedInput->setString(Mod::get()->getSavedValue<std::string>("speedhack-value", "1.0"));
        speedInput->setPosition({COL_X[1] + 15.f, GRID_TOP_Y});
        speedInput->setCallback([](std::string const& text) {
            Mod::get()->setSavedValue("speedhack-value", text);
            SpeedHack::apply();
        });
        page->addChild(speedInput);

        // Room for more Core toggles below, using the same saving system
        addToggle(page, menu, "core-example-1", "Example Toggle", 1, 0);
        addToggle(page, menu, "core-example-2", "Example Toggle", 1, 1);
    }

    // --- Cosmetic / Level / Creator: placeholder grids ready to wire up ---
    for (int t = 1; t < TAB_COUNT; t++) {
        auto page = m_pages[t];
        auto menu = m_pageMenus[t];
        int slot = 0;
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 2; col++) {
                std::string key = std::string(TAB_KEYS[t]) + "-example-" + std::to_string(slot);
                addToggle(page, menu, key, "Example Toggle", row, col);
                slot++;
            }
        }
    }

    selectTab(0);

    return true;
}

void ModMenuPopup::addTabButton(CCMenu* sidebarMenu, int index, const std::string& label, float yPos) {
    auto sprite = ButtonSprite::create(label.c_str());
    sprite->setScale(0.6f);

    auto btn = CCMenuItemExt::createSpriteExtra(sprite, [this, index](CCMenuItemSpriteExtra*) {
        this->selectTab(index);
    });
    btn->setPosition({SIDEBAR_WIDTH / 2.f, yPos});
    sidebarMenu->addChild(btn);
    m_tabButtons[index] = btn;
}

void ModMenuPopup::addToggle(CCNode* page, CCMenu* pageMenu, const std::string& saveKey,
                              const std::string& label, int row, int col) {
    float x = COL_X[col];
    float y = GRID_TOP_Y - row * ROW_HEIGHT;

    // Every toggle, in every tab, saves/loads through this same path -
    // that consistency is what makes the saving system reliable.
    auto toggle = CCMenuItemExt::createTogglerWithStandardSprites(
        0.6f,
        [saveKey](CCMenuItemToggler* t) {
            bool state = !t->isToggled();
            Mod::get()->setSavedValue(saveKey, state);
        }
    );
    toggle->toggle(Mod::get()->getSavedValue<bool>(saveKey, false));
    toggle->setPosition({x, y});
    pageMenu->addChild(toggle);

    auto lbl = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    lbl->setScale(0.35f);
    lbl->setAnchorPoint({0.f, 0.5f});
    lbl->setPosition({x + 20.f, y});
    page->addChild(lbl);
}

void ModMenuPopup::selectTab(int index) {
    for (int i = 0; i < TAB_COUNT; i++) {
        if (m_pages[i])      m_pages[i]->setVisible(i == index);
        if (m_pageMenus[i])  m_pageMenus[i]->setEnabled(i == index);
        if (m_tabButtons[i]) m_tabButtons[i]->setOpacity(i == index ? 255 : 140);
    }
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
