#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

// --- Mod Menu Popup ---
class MyModMenuPopup : public FLAlertLayer {
protected:
    bool init() {
        if (!FLAlertLayer::init(150)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Main container layer
        m_mainLayer = CCLayer::create();
        this->addChild(m_mainLayer);

        // Background panel
        auto bg = CCScale9Sprite::create("GJ_square01.png", { 0, 0, 80, 80 });
        bg->setContentSize({ 280.0f, 180.0f });
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        // Title
        auto title = CCLabelBMFont::create("Mod Menu", "bigFont.fnt");
        title->setPosition({ winSize.width / 2, (winSize.height / 2) + 65.0f });
        title->setScale(0.7f);
        m_mainLayer->addChild(title);

        // Interactive menu layer
        auto menu = CCMenu::create();
        menu->setPosition(winSize / 2);
        m_mainLayer->addChild(menu);

        // Close Button
        auto closeBtnSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        closeBtnSprite->setScale(0.8f);

        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeBtnSprite, this, menu_selector(MyModMenuPopup::onClose)
        );
        closeBtn->setPosition({ -125.0f, 75.0f });
        menu->addChild(closeBtn);

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