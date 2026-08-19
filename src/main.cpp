#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

// =======================================================================
// ModMenuPopup
//
// A sidebar of category tabs on the left + a content panel on the right
// that swaps between pages of toggles depending on the selected tab —
// the general layout style used by most GD mod menus (Eclipse, Mega
// Hack, etc). Built entirely with standard Geode/Cocos2d UI pieces, not
// copied from any specific mod's code or assets.
//
// Everything below is a skeleton with placeholder rows. To add real
// features:
//   1. Replace the row labels passed into addTab(...) in init().
//   2. Handle the real toggle logic in onRowToggled() (the sender's
//      new state is described in the comment inside it).
//
// References:
//   - Buttons:     https://docs.geode-sdk.org/tutorials/buttons/
//   - Popups:      https://docs.geode-sdk.org/tutorials/popup/
//   - Positioning: https://docs.geode-sdk.org/tutorials/positioning/
// =======================================================================
class ModMenuPopup : public geode::Popup {
protected:
    static constexpr float POPUP_WIDTH = 420.f;
    static constexpr float POPUP_HEIGHT = 280.f;

    std::vector<CCMenuItemSpriteExtra*> m_tabButtons;
    std::vector<CCMenu*> m_pages;

    // One toggle + label row inside a page.
    void addRow(CCMenu* page, char const* label, float y) {
        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

        auto toggle = CCMenuItemToggler::create(
            offSpr, onSpr, this, menu_selector(ModMenuPopup::onRowToggled)
        );
        toggle->setScale(0.65f);
        toggle->setPosition({ 20.f, y });
        page->addChild(toggle);

        auto text = CCLabelBMFont::create(label, "bigFont.fnt");
        text->setScale(0.4f);
        text->setAnchorPoint({ 0.f, 0.5f });
        text->setPosition({ 44.f, y });
        page->addChild(text);
    }

    // One sidebar tab + its content page, built from a list of row labels.
    void addTab(char const* name, std::vector<std::string> const& rows, float tabY) {
        int index = static_cast<int>(m_pages.size());

        // ---- sidebar button ----
        auto sprite = ButtonSprite::create(name);
        sprite->setScale(0.6f);

        auto button = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(ModMenuPopup::onSelectTab)
        );
        button->setTag(index);
        button->setPosition({ 55.f, tabY });
        m_mainLayer->addChild(button);
        m_tabButtons.push_back(button);

        // ---- content page (hidden until selected) ----
        auto page = CCMenu::create();
        page->setPosition({ 468.f, 160.f });
        page->ignoreAnchorPointForPosition(false);

        float y = 195.f;
        for (auto& row : rows) {
            addRow(page, row.c_str(), y);
            y -= 34.f;
        }

        m_mainLayer->addChild(page);
        m_pages.push_back(page);
    }

    void onSelectTab(CCObject* sender) {
        int index = static_cast<CCNode*>(sender)->getTag();
        for (int i = 0; i < static_cast<int>(m_pages.size()); i++) {
            m_pages[i]->setVisible(i == index);

            auto spr = static_cast<ButtonSprite*>(m_tabButtons[i]->getNormalImage());
            spr->setColor(i == index ? ccc3(255, 255, 255) : ccc3(120, 120, 120));
        }
    }

    void onRowToggled(CCObject* sender) {
        // TODO: wire this up to a real feature.
        // `sender` is the CCMenuItemToggler that was clicked; its state
        // right after the click is available via isToggled(), e.g.:
        //
        // auto toggler = static_cast<CCMenuItemToggler*>(sender);
        // bool enabled = toggler->isToggled();
    }

    bool init() {
        if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT))
            return false;

        this->setTitle("Mod Menu");

        // Add a tab (name, row labels, sidebar y-position) for each
        // category. Add/remove tabs and rows freely here.
        addTab("Player", { "Example toggle 1", "Example toggle 2" }, 195.f);
        addTab("Visual", { "Example toggle 1", "Example toggle 2" }, 155.f);
        addTab("Misc",   { "Example toggle 1", "Example toggle 2" }, 115.f);

        onSelectTab(m_tabButtons[0]);

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
// Hook the main menu (MenuLayer) to add a button that opens the mod
// menu popup. Swap MenuLayer for another layer (e.g. PlayLayer) if
// you'd rather have the button show up somewhere else in the game.
// =======================================================================
class $modify(ModMenuHook, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto buttonSprite = ButtonSprite::create("Menu");
        buttonSprite->setScale(0.6f);

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(ModMenuHook::onOpenModMenu)
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
