#pragma once
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class ModMenuPopup : public geode::Popup {
protected:
    bool init();

    void onSpeedHackToggle(CCObject* sender);
    void onToggle2(CCObject* sender);

public:
    static ModMenuPopup* create();
};
