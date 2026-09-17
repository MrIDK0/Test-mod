#include "HitboxColorsPopup.hpp"

namespace {
    constexpr float POPUP_WIDTH  = 460.f;
    constexpr float POPUP_HEIGHT = 300.f;
    constexpr float ROW_HEIGHT   = 34.f;
    constexpr float TOP_Y        = 235.f;

    constexpr float LABEL_X  = 20.f;
    constexpr float FILL_X   = 200.f;
    constexpr float BORDER_X = 290.f;
    constexpr float THICK_X  = 390.f;
}

bool HitboxColorsPopup::init() {
    if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT)) return false;
    this->setTitle("Hitbox Colors");

    auto addHeader = [&](const char* text, float x) {
        auto lbl = CCLabelBMFont::create(text, "bigFont.fnt");
        lbl->setScale(0.3f);
        lbl->setPosition({x, TOP_Y + 20.f});
        m_mainLayer->addChild(lbl);
    };
    addHeader("Fill", FILL_X);
    addHeader("Border", BORDER_X);
    addHeader("Width", THICK_X);

    addCategoryRow(HitboxColors::Category::Hazard,      TOP_Y);
    addCategoryRow(HitboxColors::Category::Solid,       TOP_Y - ROW_HEIGHT);
    addCategoryRow(HitboxColors::Category::Player1,     TOP_Y - 2.f * ROW_HEIGHT);
    addCategoryRow(HitboxColors::Category::Player2,     TOP_Y - 3.f * ROW_HEIGHT);
    addCategoryRow(HitboxColors::Category::Interactive, TOP_Y - 4.f * ROW_HEIGHT);

    // Global fill opacity - independent of border/stroke opacity, which
    // always stays fully opaque.
    float opacityY = TOP_Y - 5.f * ROW_HEIGHT - 12.f;

    auto opLabel = CCLabelBMFont::create("Global Fill Opacity (0-1)", "bigFont.fnt");
    opLabel->setScale(0.35f);
    opLabel->setAnchorPoint({0.f, 0.5f});
    opLabel->setPosition({LABEL_X, opacityY});
    m_mainLayer->addChild(opLabel);

    auto opInput = TextInput::create(60.f, "0.2", "bigFont.fnt");
    opInput->setFilter("0123456789.");
    opInput->setMaxCharCount(4);
    opInput->setString(Mod::get()->getSavedValue<std::string>("hitboxcolor-global-fillopacity", "0.2"));
    opInput->setPosition({POPUP_WIDTH - 60.f, opacityY});
    opInput->setCallback([](std::string const& text) {
        Mod::get()->setSavedValue("hitboxcolor-global-fillopacity", text);
    });
    m_mainLayer->addChild(opInput);

    return true;
}

void HitboxColorsPopup::addCategoryRow(HitboxColors::Category cat, float y) {
    std::string catKey = HitboxColors::categoryKey(cat);
    std::string defaultHex = HitboxColors::defaultHex(cat);

    auto lbl = CCLabelBMFont::create(HitboxColors::categoryLabel(cat).c_str(), "bigFont.fnt");
    lbl->setScale(0.35f);
    lbl->setAnchorPoint({0.f, 0.5f});
    lbl->setPosition({LABEL_X, y});
    m_mainLayer->addChild(lbl);

    auto fillInput = TextInput::create(65.f, "FFFFFF", "bigFont.fnt");
    fillInput->setFilter("0123456789ABCDEFabcdef");
    fillInput->setMaxCharCount(6);
    fillInput->setString(Mod::get()->getSavedValue<std::string>("hitboxcolor-" + catKey + "-fill", defaultHex));
    fillInput->setPosition({FILL_X, y});
    fillInput->setCallback([catKey](std::string const& text) {
        Mod::get()->setSavedValue("hitboxcolor-" + catKey + "-fill", text);
    });
    m_mainLayer->addChild(fillInput);

    auto borderInput = TextInput::create(65.f, "FFFFFF", "bigFont.fnt");
    borderInput->setFilter("0123456789ABCDEFabcdef");
    borderInput->setMaxCharCount(6);
    borderInput->setString(Mod::get()->getSavedValue<std::string>("hitboxcolor-" + catKey + "-border", defaultHex));
    borderInput->setPosition({BORDER_X, y});
    borderInput->setCallback([catKey](std::string const& text) {
        Mod::get()->setSavedValue("hitboxcolor-" + catKey + "-border", text);
    });
    m_mainLayer->addChild(borderInput);

    auto thickInput = TextInput::create(55.f, "0.25", "bigFont.fnt");
    thickInput->setFilter("0123456789.");
    thickInput->setMaxCharCount(4);
    thickInput->setString(Mod::get()->getSavedValue<std::string>("hitboxcolor-" + catKey + "-thickness", "0.25"));
    thickInput->setPosition({THICK_X, y});
    thickInput->setCallback([catKey](std::string const& text) {
        Mod::get()->setSavedValue("hitboxcolor-" + catKey + "-thickness", text);
    });
    m_mainLayer->addChild(thickInput);
}

HitboxColorsPopup* HitboxColorsPopup::create() {
    auto ret = new HitboxColorsPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
