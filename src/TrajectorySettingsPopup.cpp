#include "TrajectorySettingsPopup.hpp"

namespace {
    constexpr float POPUP_WIDTH  = 380.f;
    constexpr float POPUP_HEIGHT = 260.f;
    constexpr float ROW_HEIGHT   = 36.f;
    constexpr float TOP_Y        = 210.f;
    constexpr float LABEL_X      = 20.f;
    constexpr float INPUT_X      = POPUP_WIDTH - 70.f;
}

bool TrajectorySettingsPopup::init() {
    if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT)) return false;
    this->setTitle("Trajectory Settings");

    auto addRow = [&](const char* label, const std::string& key, const std::string& defaultVal,
                       const char* filter, int maxChars, float y) {
        auto lbl = CCLabelBMFont::create(label, "bigFont.fnt");
        lbl->setScale(0.35f);
        lbl->setAnchorPoint({0.f, 0.5f});
        lbl->setPosition({LABEL_X, y});
        m_mainLayer->addChild(lbl);

        auto input = TextInput::create(65.f, defaultVal.c_str(), "bigFont.fnt");
        input->setFilter(filter);
        input->setMaxCharCount(maxChars);
        input->setString(Mod::get()->getSavedValue<std::string>(key, defaultVal));
        input->setPosition({INPUT_X, y});
        input->setCallback([key](std::string const& text) {
            Mod::get()->setSavedValue(key, text);
        });
        m_mainLayer->addChild(input);
    };

    addRow("Iterations",        "trajectory-iterations",    "150",    "0123456789", 4, TOP_Y);
    addRow("Line Thickness",    "trajectory-thickness",     "0.65",   "0123456789.", 4, TOP_Y - ROW_HEIGHT);
    addRow("Normal Path Color", "trajectory-normal-color",  "00FFFF", "0123456789ABCDEFabcdef", 6, TOP_Y - 2.f * ROW_HEIGHT);
    addRow("Jump Path Color",   "trajectory-jump-color",    "E900FF", "0123456789ABCDEFabcdef", 6, TOP_Y - 3.f * ROW_HEIGHT);
    addRow("Death Point Color", "trajectory-death-color",   "FF0000", "0123456789ABCDEFabcdef", 6, TOP_Y - 4.f * ROW_HEIGHT);

    return true;
}

TrajectorySettingsPopup* TrajectorySettingsPopup::create() {
    auto ret = new TrajectorySettingsPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
