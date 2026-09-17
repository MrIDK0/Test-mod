#pragma once
#include <Geode/loader/Mod.hpp>
#include "HitboxColors.hpp"

using namespace geode::prelude;

namespace Trajectory {
    inline bool isEnabled() {
        return Mod::get()->getSavedValue<bool>("trajectory-enabled", false);
    }

    // How many physics steps to simulate forward
    inline int getIterations() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("trajectory-iterations", "150");
        int iterations = 150;
        try { iterations = std::stoi(valueStr); } catch (...) { iterations = 150; }
        if (iterations < 10) iterations = 10;
        if (iterations > 5000000) iterations = 5000000; // sane guard - this many
                                                    // real update() calls
                                                    // per real frame adds up
        return iterations;
    }

    inline float getLineThickness() {
        std::string valueStr = Mod::get()->getSavedValue<std::string>("trajectory-thickness", "0.65");
        float thickness = 0.65f;
        try { thickness = std::stof(valueStr); } catch (...) { thickness = 0.65f; }
        if (thickness < 0.05f) thickness = 0.05f;
        if (thickness > 5.f) thickness = 5.f;
        return thickness;
    }

    inline ccColor4F getNormalColor() {
        std::string hex = Mod::get()->getSavedValue<std::string>("trajectory-normal-color", "00FFFF");
        auto rgb = HitboxColors::parseHex(hex);
        return {rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f};
    }

    inline ccColor4F getJumpColor() {
        std::string hex = Mod::get()->getSavedValue<std::string>("trajectory-jump-color", "E900FF");
        auto rgb = HitboxColors::parseHex(hex);
        return {rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f};
    }

    inline ccColor4F getDeathColor() {
        std::string hex = Mod::get()->getSavedValue<std::string>("trajectory-death-color", "FF0000");
        auto rgb = HitboxColors::parseHex(hex);
        return {rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f};
    }
}
