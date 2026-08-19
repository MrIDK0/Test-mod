#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

enum class MenuTab {
    Core,
    Cosmetic,
    Level,
    Creator
};

class MyModMenuPopup : public FLAlertLayer {
private:
    CCLayer* m_contentLayer = nullptr;
    MenuTab m_currentTab = MenuTab::Core;

    // Helper method to add a single toggle + label to a menu container
    void addToggle(CCMenu* menu, CCPoint pos, std::string const& text, std::string const& settingKey) {
        auto toggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenuPopup::onToggleSetting), 0.7f
        );
        toggle->setPosition(pos);
        toggle->setUserData(new std::string(settingKey));
        
        // Read saved setting or default to false
        bool enabled = Mod::get()->getSettingValue<bool>(settingKey);
        toggle->toggle(enabled);
        menu->addChild(toggle);

        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setScale(0.35f);
        label->setAnchorPoint({0.0f, 0.5f});
        label->setPosition({pos.x + 18.0f, pos.y});
        menu->addChild(label);
    }

    void onToggleSetting(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        auto keyPtr = static_cast<std::string*>(toggle->getUserData());
        if (keyPtr) {
            bool current = Mod::get()->getSettingValue<bool>(*keyPtr);
            Mod::get()->setSettingValue(*keyPtr, !current);
        }
    }

    void onSelectTab(CCObject* sender) {
        auto btn = static_cast<CCMenuItemTagData*>(sender);
        m_currentTab = static_cast<MenuTab>(btn->getTag());
        loadTabContent();
    }

    void loadTabContent() {
        if (!m_contentLayer) return;
        m_contentLayer->removeAllChildrenWithCleanup(true);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        m_contentLayer->addChild(menu);

        // Grid positions relative to the content area center (2 columns, 6 rows max)
        std::vector<CCPoint> leftColumn = {
            {-30.0f, 60.0f}, {-30.0f, 35.0f}, {-30.0f, 10.0f},
            {-30.0f, -15.0f}, {-30.0f, -40.0f}, {-30.0f, -65.0f}
        };

        std::vector<CCPoint> rightColumn = {
            {70.0f, 60.0f}, {70.0f, 35.0f}, {70.0f, 10.0f},
            {70.0f, -15.0f}, {70.0f, -40.0f}, {70.0f, -65.0f}
        };

        // Populate items based on selected tab
        std::string tabPrefix = "core";
        switch (m_currentTab) {
            case MenuTab::Core:     tabPrefix = "core"; break;
            case MenuTab::Cosmetic: tabPrefix = "cosmetic"; break;
            case MenuTab::Level:    tabPrefix = "level"; break;
            case MenuTab::Creator:  tabPrefix = "creator"; break;
        }

        for (size_t i = 0; i < 6; ++i) {
            addToggle(menu, leftColumn[i], "Example toggle", tabPrefix + "_left_" + std::to_string(i));
            addToggle(menu, rightColumn[i], "Example toggle", tabPrefix + "_right_" + std::to_string(i));
        }
    }

protected:
    bool init() override {
        if (!FLAlertLayer::init(150)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_mainLayer = CCLayer::create();
        this->addChild(m_mainLayer);

        // Main Dialog Background (Width: 380, Height: 220)
        auto bg = CCScale9Sprite::create("GJ_square01.png", { 0, 0, 80, 80 });
        bg->setContentSize({ 380.0f, 220.0f });
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        // Header Title Banner
        auto titleBg = CCScale9Sprite::create("GJ_square02.png", {0, 0, 80, 80});
        titleBg->setContentSize({120.0f, 30.0f});
        titleBg->setPosition({winSize.width / 2, (winSize.height / 2) + 110.0f});
        m_mainLayer->addChild(titleBg);

        auto title = CCLabelBMFont::create("Mod menu", "bigFont.fnt");
        title->setPosition(titleBg->getPosition());
        title->setScale(0.5f);
        m_mainLayer->addChild(title);

        // Close Button
        auto closeMenu = CCMenu::create();
        closeMenu->setPosition({0, 0});
        m_mainLayer->addChild(closeMenu);

        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this,
            menu_selector(MyModMenuPopup::onClose)
        );
        closeBtn->setPosition({(winSize.width / 2) - 175.0f, (winSize.height / 2) + 95.0f});
        closeMenu->addChild(closeBtn);

        // Sidebar Panel Background
        auto sidebarBg = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
        sidebarBg->setContentSize({90.0f, 195.0f});
        sidebarBg->setPosition({(winSize.width / 2) - 130.0f, winSize.height / 2});
        sidebarBg->setOpacity(100);
        m_mainLayer->addChild(sidebarBg);

        // Sidebar Navigation Buttons
        auto sidebarMenu = CCMenu::create();
        sidebarMenu->setPosition(sidebarBg->getPosition());
        m_mainLayer->addChild(sidebarMenu);

        std::vector<std::pair<std::string, MenuTab>> tabs = {
            {"Core", MenuTab::Core},
            {"Cosmetic", MenuTab::Cosmetic},
            {"Level", MenuTab::Level},
            {"Creator", MenuTab::Creator}
        };

        float startY = 65.0f;
        for (auto const& tab : tabs) {
            auto spr = ButtonSprite::create(tab.first.c_str(), 70, true, "goldFont.fnt", "GJ_button_01.png", 30.0f, 0.5f);
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(MyModMenuPopup::onSelectTab)
            );
            btn->setTag(static_cast<int>(tab.second));
            btn->setPosition({0.0f, startY});
            sidebarMenu->addChild(btn);

            startY -= 43.0f;
        }

        // Content Area Container
        m_contentLayer = CCLayer::create();
        m_contentLayer->setPosition({(winSize.width / 2) + 40.0f, winSize.height / 2});
        m_mainLayer->addChild(m_contentLayer);

        loadTabContent();
        return true;
    }

    void onClose(CCObject* sender) {
        this->keyBackClicked();
    }

public:
    static MyModMenuPopup* create() {
        auto ret = new MyModMenuPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void show() {
        FLAlertLayer::show();
    }
};

// --- PauseLayer Hook ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) return;

        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        btnSprite->setScale(0.65f);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(MyPauseLayer::onOpenModMenu)
        );
        btn->setID("mod-menu-button"_spr);

        menu->addChild(btn);
        menu->updateLayout();
    }

    void onOpenModMenu(CCObject* sender) {
        MyModMenuPopup::create()->show();
    }
};