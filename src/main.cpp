#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

// --- Custom Trajectory Node ---
class TrajectoryNode : public CCNode {
private:
    CCDrawNode* m_drawNode = nullptr;

public:
    CREATE_FUNC(TrajectoryNode);

    bool init() override {
        if (!CCNode::init()) return false;
        
        m_drawNode = CCDrawNode::create();
        this->addChild(m_drawNode);
        return true;
    }

    void drawTrajectory(PlayerObject* player) {
        m_drawNode->clear();

        if (!player || !Mod::get()->getSettingValue<bool>("show-trajectory")) {
            return;
        }

        // Current kinematics state
        CCPoint pos = player->getPosition();
        CCPoint vel = player->m_playerSpeed;
        double gravity = player->m_gravity;
        
        const int steps = 40;
        const float stepTime = 1.0f / 60.0f;
        
        CCPoint prevPos = pos;

        // Simple Euler integration projection
        for (int i = 0; i < steps; ++i) {
            // Apply gravity vector
            vel.y += gravity * stepTime;
            
            // Advance predicted position
            pos.x += vel.x * stepTime;
            pos.y += vel.y * stepTime;

            // Draw line segment from last point to projected point (Cyan, 2px)
            m_drawNode->drawLine(
                prevPos, 
                pos, 
                ccc4f(0.0f, 1.0f, 1.0f, 0.8f), 
                2.0f
            );

            prevPos = pos;
        }
    }
};

// --- PlayLayer Hook ---
class $modify(MyPlayLayer, PlayLayer) {
    TrajectoryNode* m_trajectoryNode = nullptr;

    bool init(GJGameLevel* level, bool useReplay, bool dontSetVisible) {
        if (!PlayLayer::init(level, useReplay, dontSetVisible)) return false;

        // Attach trajectory overlay to the object layer
        auto trajectory = TrajectoryNode::create();
        trajectory->setID("trajectory-node"_spr);
        m_objectLayer->addChild(trajectory, 100);
        m_fields->m_trajectoryNode = trajectory;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        if (m_fields->m_trajectoryNode && m_player1) {
            m_fields->m_trajectoryNode->drawTrajectory(m_player1);
        }
    }
};

// --- Mod Menu Popup ---
class MyModMenuPopup : public Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mod Menu");

        auto menu = CCMenu::create();
        menu->setPosition(m_mainLayer->getContentSize() / 2);
        m_mainLayer->addChild(menu);

        // Toggle 1: Noclip
        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenuPopup::onToggleNoclip), 0.8f
        );
        noclipToggle->setPosition({-60, 20});
        noclipToggle->toggle(Mod::get()->getSettingValue<bool>("noclip-enabled"));
        menu->addChild(noclipToggle);

        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.5f);
        noclipLabel->setPosition({noclipToggle->getPositionX() + 60, noclipToggle->getPositionY()});
        menu->addChild(noclipLabel);

        // Toggle 2: Trajectory Path
        auto trajToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenuPopup::onToggleTrajectory), 0.8f
        );
        trajToggle->setPosition({-60, -30});
        trajToggle->toggle(Mod::get()->getSettingValue<bool>("show-trajectory"));
        menu->addChild(trajToggle);

        auto trajLabel = CCLabelBMFont::create("Trajectory Path", "bigFont.fnt");
        trajLabel->setScale(0.5f);
        trajLabel->setPosition({trajToggle->getPositionX() + 95, trajToggle->getPositionY()});
        menu->addChild(trajLabel);

        return true;
    }

    void onToggleNoclip(CCObject* sender) {
        bool current = Mod::get()->getSettingValue<bool>("noclip-enabled");
        Mod::get()->setSettingValue("noclip-enabled", !current);
    }

    void onToggleTrajectory(CCObject* sender) {
        bool current = Mod::get()->getSettingValue<bool>("show-trajectory");
        Mod::get()->setSettingValue("show-trajectory", !current);
    }

public:
    static MyModMenuPopup* create() {
        auto ret = new MyModMenuPopup();
        if (ret && ret->initAnchored(280.0f, 180.0f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
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

// --- PlayerObject Hook ---
class $modify(MyPlayerObject, PlayerObject) {
    void destroyPlayer(bool p0, GameObject* p1) {
        if (Mod::get()->getSettingValue<bool>("noclip-enabled")) {
            return;
        }
        PlayerObject::destroyPlayer(p0, p1);
    }
};
