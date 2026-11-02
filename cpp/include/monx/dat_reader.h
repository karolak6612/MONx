#pragma once

#include "monx/formats_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

namespace monx {

struct ThingType {
    Category category;
    uint32_t id = 0;
    uint8_t width = 1;
    uint8_t height = 1;
    uint8_t exact_size = 32;
    uint8_t layers = 1;
    uint8_t pattern_x = 1;
    uint8_t pattern_y = 1;
    uint8_t pattern_z = 1;
    uint8_t frames = 1;
    std::vector<uint32_t> sprite_ids;
};

class DatReader {
public:
    static std::optional<DatReader> open(const std::string& path);

    const ThingType* get_thing(Category cat, uint32_t id) const;

private:
    uint32_t m_signature = 0;
    std::unordered_map<uint32_t, ThingType> m_items;
    std::unordered_map<uint32_t, ThingType> m_outfits;
    std::unordered_map<uint32_t, ThingType> m_effects;
    std::unordered_map<uint32_t, ThingType> m_missiles;
};

} // namespace monx
