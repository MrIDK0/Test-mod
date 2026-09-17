#pragma once
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include "HitboxColors.hpp"

using namespace geode::prelude;

class HitboxColorsPopup : public geode::Popup {
protected:
    bool init();
    void addCategoryRow(HitboxColors::Category cat, float y);

public:
    static HitboxColorsPopup* create();
};
