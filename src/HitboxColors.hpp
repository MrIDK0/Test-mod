#pragma once
#include <Geode/loader/Mod.hpp>
#include <string>
#include <algorithm>

using namespace geode::prelude;

namespace HitboxColors {
    enum class Category { Hazard, Solid, Player1, Player2, Interactive };

    inline std::string categoryKey(Category c) {
        switch (c) {
            case Category::Hazard:      return "hazard";
            case Category::Solid:       return "solid";
            case Category::Player1:     return "player1";
            case Category::Player2:     return "player2";
            case Category::Interactive: return "interactive";
        }
        return "hazard";
    }

    inline std::string categoryLabel(Category c) {
        switch (c) {
            case Category::Hazard:      return "Hazards";
            case Category::Solid:       return "Solids";
            case Category::Player1:     return "Player 1";
            case Category::Player2:     return "Player 2";
            case Category::Interactive: return "Interactives";
        }
        return "";
    }

    // Sensible defaults matching the original fixed color scheme
    inline std::string defaultHex(Category c) {
        switch (c) {
            case Category::Hazard:      return "FF0000";
            case Category::Solid:       return "0040FF";
            case Category::Player1:     return "FF0000";
            case Category::Player2:     return "0040FF";
            case Category::Interactive: return "FF00E6";
        }
        return "FFFFFF";
    }

    inline ccColor3B parseHex(const std::string& hexIn) {
        std::string hex = hexIn;
        if (!hex.empty() && hex[0] == '#') hex = hex.substr(1);
        if (hex.size() != 6) return {255, 255, 255};

        auto byteAt = [&](size_t pos) -> int {
            try { return std::stoi(hex.substr(pos, 2), nullptr, 16); }
            catch (...) { return 255; }
        };

        int r = std::clamp(byteAt(0), 0, 255);
        int g = std::clamp(byteAt(2), 0, 255);
        int b = std::clamp(byteAt(4), 0, 255);
        return {static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)};
    }

    inline ccColor4F getFillColor(Category c) {
        std::string hex = Mod::get()->getSavedValue<std::string>(
            "hitboxcolor-" + categoryKey(c) + "-fill", defaultHex(c));
        auto rgb = parseHex(hex);
        return {rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f};
    }

    inline ccColor4F getBorderColor(Category c) {
        std::string hex = Mod::get()->getSavedValue<std::string>(
            "hitboxcolor-" + categoryKey(c) + "-border", defaultHex(c));
        auto rgb = parseHex(hex);
        return {rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f};
    }

    // Independent per-category border thickness
    inline float getBorderThickness(Category c) {
        std::string valueStr = Mod::get()->getSavedValue<std::string>(
            "hitboxcolor-" + categoryKey(c) + "-thickness", "0.25");
        float thickness = 0.25f;
        try { thickness = std::stof(valueStr); } catch (...) { thickness = 0.25f; }
        if (thickness < 0.f) thickness = 0.f;
        if (thickness > 5.f) thickness = 5.f; // sane guard
        return thickness;
    }

    // Global fill opacity - one shared slider-equivalent, independent of
    // border/stroke opacity (which stays fully opaque).
    inline float getGlobalFillOpacity() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("hitboxcolor-global-fillopacity", "0.2");
        float opacity = 0.2f;
        try { opacity = std::stof(valueStr); } catch (...) { opacity = 0.2f; }
        if (opacity < 0.f) opacity = 0.f;
        if (opacity > 1.f) opacity = 1.f;
        return opacity;
    }
}
