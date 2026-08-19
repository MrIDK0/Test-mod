#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <map>
#include <string>
#include <vector>

using namespace geode::prelude;

class CustomModMenu : public geode::Popup<std::string const&> {
protected:
    std::map<std::string, CCNode*> m_categoryContainers;
    std::map<std::string, ButtonSprite*> m_categoryButtons;
    std::string m_currentCategory = "CORE";

    bool setup(std::string const& title) override {
        this->setTitle("MOD MENU");

        // Main Background Box
        auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize({380.f, 210.f});
        bg->setPosition({m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f - 10.f});
        m_mainLayer->addChild(bg);

        // Sidebar Background
        auto sidebarBg = cocos2d::extension::CCScale9Sprite::create("square02_001.png");
        sidebarBg->setContentSize({100.f, 190.f});
        sidebarBg->setPosition({bg->getPositionX() - 130.f, bg->getPositionY()});
        sidebarBg->setOpacity(100);
        m_mainLayer->addChild(sidebarBg);

        // 1. Sidebar Category Buttons
        auto sidebarMenu = CCMenu::create();
        sidebarMenu->setPosition({sidebarBg->getPositionX(), sidebarBg->getPositionY()});
        sidebarMenu->setContentSize({100.f, 180.f});
        sidebarMenu->setLayout(
            ColumnLayout::create()
                ->setGap(8.f)
                ->setAxisReverse(true)
        );

        std::vector<std::string> categories = {"CORE", "COSMETIC", "LEVEL", "CREATOR"};

        for (const auto& catName : categories) {
            auto btnSprite = ButtonSprite::create(catName.c_str(), 80, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
            
            auto btn = CCMenuItemSpriteExtra::create(
                btnSprite,
                this,
                menu_selector(CustomModMenu::onCategorySelect)
            );
            btn->setID(catName);

            m_categoryButtons[catName] = btnSprite;
            sidebarMenu->addChild(btn);
            
            // Category Content Container
            auto categoryContainer = CCMenu::create();
            categoryContainer->setPosition({bg->getPositionX() + 50.f, bg->getPositionY()});
            categoryContainer->setContentSize({240.f, 180.f});
            
            // Row-based multi-row wrapping layout
            categoryContainer->setLayout(
                RowLayout::create()
                    ->setGap(10.f)
                    ->setGrowCrossAxis(true)
            );

            this->populateCategoryToggles(categoryContainer, catName);
            
            categoryContainer->updateLayout();
            categoryContainer->setVisible(catName == m_currentCategory);
            
            m_mainLayer->addChild(categoryContainer);
            m_categoryContainers[catName] = categoryContainer;
        }

        sidebarMenu->updateLayout();
        m_mainLayer->addChild(sidebarMenu);

        this->updateTabVisuals();

        return true;
    }

    void populateCategoryToggles(CCMenu* container, const std::string& catName) {
        for (int i = 1; i <= 8; i++) {
            auto toggle = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(CustomModMenu::onToggle),
                0.6f
            );

            std::string labelText = catName + " OPT " + std::to_string(i);
            auto label = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
            label->setScale(0.3f);
            label->setAnchorPoint({0.f, 0.5f});

            auto itemNode = CCNode::create();
            itemNode->setContentSize({110.f, 25.f});

            toggle->setPosition({12.f, 12.5f});
            label->setPosition({28.f, 12.5f});

            itemNode->addChild(toggle);
            itemNode->addChild(label);

            container->addChild(itemNode);
        }
    }

    void onCategorySelect(CCObject* sender) {
        auto btn = static_cast<CCNode*>(sender);
        std::string selectedCategory = btn->getID();

        if (selectedCategory == m_currentCategory) return;

        if (m_categoryContainers.count(m_currentCategory)) {
            m_categoryContainers[m_currentCategory]->setVisible(false);
        }

        m_currentCategory = selectedCategory;
        if (m_categoryContainers.count(m_currentCategory)) {
            m_categoryContainers[m_currentCategory]->setVisible(true);
        }

        this->updateTabVisuals();
    }

    void updateTabVisuals() {
        for (auto& [catName, btnSprite] : m_categoryButtons) {
            if (catName == m_currentCategory) {
                btnSprite->updateBGImage("GJ_button_02.png");
            } else {
                btnSprite->updateBGImage("GJ_button_01.png");
            }
        }
    }

    void onToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        bool isToggled = !toggle->isToggled();
    }

public:
    static CustomModMenu* create() {
        auto ret = new CustomModMenu();
        if (ret && ret->initAnchored(400.f, 260.f, "MOD MENU")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
