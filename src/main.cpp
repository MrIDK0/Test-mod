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

    void addToggle(CCMenu* menu, CCPoint pos, std::string const& text, std::string const& settingKey) {
        auto toggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenuPopup::onToggleSetting), 0.65f
        );
        toggle->setPosition(pos);
        toggle->setUserData(new std::string(settingKey));
        
        bool enabled = Mod::get()->getSettingValue<bool>(settingKey);
        toggle->toggle(enabled);
        menu->addChild(toggle);

        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setScale(0.28f);
        label->setAnchorPoint({0.0f, 0.5f});
        label->setPosition({pos.x + 14.0f, pos.y});
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
        auto btn = static_cast<CCMenuItem*>(sender);
        if (!btn) return;

        m_currentTab = static_cast<MenuTab>(btn->getTag());
        loadTabContent();
    }

    void loadTabContent() {
        if (!m_contentLayer) return;
        m_contentLayer->removeAllChildrenWithCleanup(true);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        m_contentLayer->addChild(menu);

        // Adjust coordinates relative to m_contentLayer center
        std::vector<CCPoint> leftColumn = {
            {-65.0f, 60.0f}, {-65.0f, 35.0f}, {-65.0f, 10.0f},
            {-65.0f, -15.0f}, {-65.0f, -40.0f}, {-65.0f, -65.0f}
        };

        std::vector<CCPoint> rightColumn = {
            {35.0f, 60.0f}, {35.0f, 35.0f}, {35.0f, 10.0f},
            {35.0f, -15.0f}, {35.0f, -40.0f}, {35.0f, -65.0f}
        };

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

        // Main Dialog Background
        auto bg = CCScale9Sprite::create("GJ_square01.png", { 0, 0, 80, 80 });
        bg->setContentSize({ 380.0f, 220.0f });
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        // Header Title Banner
        auto titleBg = CCScale9Sprite::create("GJ_square02.png", {0, 0, 80, 80});
        titleBg->setContentSize({120.0f, 30.0f});
        titleBg->setPosition({winSize.width / 2, (winSize.height / 2) + 110.0f});
        m_mainLayer->addChild(titleBg);

        auto title = CCLabelBMFont::create("MOD MENU", "bigFont.fnt");
        title->setPosition(titleBg->getPosition());
        title->setScale(0.5f);
        m_mainLayer->addChild(title);

        // Close Button (Anchored to top-left corner of the background)
        auto closeMenu = CCMenu::create();
        closeMenu->setPosition({0, 0});
        m_mainLayer->addChild(closeMenu);

        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this,
            menu_selector(MyModMenuPopup::onClose)
        );
        closeBtn->setPosition({(winSize.width / 2) - 180.0f, (winSize.height / 2) + 100.0f});
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
            {"CORE", MenuTab::Core},
            {"COSMETIC", MenuTab::Cosmetic},
            {"LEVEL", MenuTab::Level},
            {"CREATOR", MenuTab::Creator}
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

        // Content Area Container (Shifted left to center inside the right box area)
        m_contentLayer = CCLayer::create();
        m_contentLayer->setPosition({(winSize.width / 2) + 70.0f, winSize.height / 2});
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

    void show() override {
        FLAlertLayer::show();
    }
};

// --- PauseLayer Hook ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("left-button-menu");
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
