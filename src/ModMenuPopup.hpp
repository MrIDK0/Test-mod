#pragma once
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/cocos.hpp>
#include <array>
#include <string>

using namespace geode::prelude;

class ModMenuPopup : public geode::Popup {
protected:
    static constexpr int TAB_COUNT = 4;

    std::array<CCNode*, TAB_COUNT> m_pages{};
    std::array<CCMenu*, TAB_COUNT> m_pageMenus{};
    std::array<CCMenuItemSpriteExtra*, TAB_COUNT> m_tabButtons{};

    bool init();

    // Builds one sidebar tab button
    void addTabButton(CCMenu* sidebarMenu, int index, const std::string& label, float yPos);

    // Generic save-bound toggle: reads/writes Mod::get()'s saved value under
    // saveKey automatically. This is the "saving system" - every toggle in
    // every tab goes through this one function, so behavior is consistent
    // everywhere instead of being copy-pasted per feature.
    void addToggle(CCNode* page, CCMenu* pageMenu, const std::string& saveKey,
                    const std::string& label, int row, int col);

    void selectTab(int index);

public:
    static ModMenuPopup* create();
};
