#pragma once
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class ModMenuPopup : public geode::Popup {
protected:
    bool init();

    void onToggle1(CCObject* sender);
    void onToggle2(CCObject* sender);

public:
    static ModMenuPopup* create();
};
