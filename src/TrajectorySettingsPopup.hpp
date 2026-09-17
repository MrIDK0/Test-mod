#pragma once
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include "Trajectory.hpp"

using namespace geode::prelude;

class TrajectorySettingsPopup : public geode::Popup {
protected:
    bool init();

public:
    static TrajectorySettingsPopup* create();
};
