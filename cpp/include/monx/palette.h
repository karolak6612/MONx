#pragma once

#include <cstdint>
#include <tuple>
#include <cmath>

namespace monx {

constexpr uint32_t HSI_H_STEPS = 19;
constexpr uint32_t HSI_SI_VALUES = 7;

inline std::tuple<uint8_t, uint8_t, uint8_t> outfit_palette_color(uint32_t index) {
    if (index >= HSI_H_STEPS * HSI_SI_VALUES) index = 0;

    float hue = 0.0f, sat = 0.0f, val = 0.0f;
    if (index % HSI_H_STEPS == 0) {
        hue = 0.0f;
        sat = 0.0f;
        val = 1.0f - (float)(index / HSI_H_STEPS) / (float)HSI_SI_VALUES;
    } else {
        hue = (float)(index % HSI_H_STEPS) / (float)(HSI_H_STEPS - 1);
        switch (index / HSI_H_STEPS) {
            case 0: sat = 0.25f; val = 1.00f; break;
            case 1: sat = 0.50f; val = 1.00f; break;
            case 2: sat = 0.75f; val = 1.00f; break;
            case 3: sat = 1.00f; val = 1.00f; break;
            case 4: sat = 1.00f; val = 0.75f; break;
            case 5: sat = 1.00f; val = 0.50f; break;
            case 6: sat = 1.00f; val = 0.25f; break;
            default: sat = 1.0f; val = 1.0f; break;
        }
    }

    // HSV to RGB conversion
    int hi = (int)std::floor(hue * 6.0f);
    float f = hue * 6.0f - (float)hi;
    float p = val * (1.0f - sat);
    float q = val * (1.0f - f * sat);
    float t = val * (1.0f - (1.0f - f) * sat);

    float r = 0.0f, g = 0.0f, b = 0.0f;
    switch (hi % 6) {
        case 0: r = val; g = t; b = p; break;
        case 1: r = q; g = val; b = p; break;
        case 2: r = p; g = val; b = t; break;
        case 3: r = p; g = q; b = val; break;
        case 4: r = t; g = p; b = val; break;
        case 5: r = val; g = p; b = q; break;
    }

    return {
        (uint8_t)std::clamp(r * 255.0f, 0.0f, 255.0f),
        (uint8_t)std::clamp(g * 255.0f, 0.0f, 255.0f),
        (uint8_t)std::clamp(b * 255.0f, 0.0f, 255.0f)
    };
}

} // namespace monx
