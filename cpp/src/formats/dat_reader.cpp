#include "monx/dat_reader.h"
#include <fstream>
#include <cstring>
#include <iostream>

namespace monx {

std::optional<DatReader> DatReader::open(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return std::nullopt;

    size_t size = f.tellg();
    if (size < 12) return std::nullopt;
    f.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    if (!f.read(reinterpret_cast<char*>(data.data()), size)) return std::nullopt;

    DatReader reader;
    std::memcpy(&reader.m_signature, data.data(), 4);

    uint16_t item_count = 0, outfit_count = 0, effect_count = 0, missile_count = 0;
    std::memcpy(&item_count, data.data() + 4, 2);
    std::memcpy(&outfit_count, data.data() + 6, 2);
    std::memcpy(&effect_count, data.data() + 8, 2);
    std::memcpy(&missile_count, data.data() + 10, 2);

    size_t pos = 12;

    auto read_thing = [&](Category cat, uint32_t id) -> bool {
        ThingType t;
        t.category = cat;
        t.id = id;

        // Skip flags until end byte 0xFF
        while (pos < size) {
            uint8_t flag = data[pos++];
            if (flag == 0xFF) break;

            // Common flag skips:
            if (flag == 0x00 || flag == 0x07 || flag == 0x08 || flag == 0x13 || flag == 0x16 || flag == 0x1A || flag == 0x19 || flag == 0x1C) {
                pos += 2; // u16 payload
            } else if (flag == 0x10 || flag == 0x15 || flag == 0x18) {
                pos += 4; // u16x2 payload
            }
        }

        if (pos + 7 > size) return false;

        t.width = data[pos++];
        t.height = data[pos++];
        if (t.width > 1 || t.height > 1) {
            t.exact_size = data[pos++];
        } else {
            t.exact_size = 32;
        }
        t.layers = data[pos++];
        t.pattern_x = data[pos++];
        t.pattern_y = data[pos++];
        t.pattern_z = data[pos++];
        t.frames = data[pos++];

        size_t total_sprites = (size_t)t.width * t.height * t.layers * t.pattern_x * t.pattern_y * t.pattern_z * t.frames;
        if (total_sprites > 4096) return false;

        for (size_t s = 0; s < total_sprites; ++s) {
            if (pos + 2 > size) return false;
            uint16_t sprite_id = data[pos] | (data[pos + 1] << 8);
            pos += 2;
            t.sprite_ids.push_back(sprite_id);
        }

        if (cat == Category::Item) reader.m_items[id] = t;
        else if (cat == Category::Outfit) reader.m_outfits[id] = t;
        else if (cat == Category::Effect) reader.m_effects[id] = t;
        else if (cat == Category::Missile) reader.m_missiles[id] = t;

        return true;
    };

    // Parse items
    for (uint32_t id = 100; id <= item_count && pos < size; ++id) {
        if (!read_thing(Category::Item, id)) break;
    }
    // Parse outfits
    for (uint32_t id = 1; id <= outfit_count && pos < size; ++id) {
        if (!read_thing(Category::Outfit, id)) break;
    }
    // Parse effects
    for (uint32_t id = 1; id <= effect_count && pos < size; ++id) {
        if (!read_thing(Category::Effect, id)) break;
    }
    // Parse missiles
    for (uint32_t id = 1; id <= missile_count && pos < size; ++id) {
        if (!read_thing(Category::Missile, id)) break;
    }

    return reader;
}

const ThingType* DatReader::get_thing(Category cat, uint32_t id) const {
    const std::unordered_map<uint32_t, ThingType>* map = nullptr;
    switch (cat) {
        case Category::Item: map = &m_items; break;
        case Category::Outfit: map = &m_outfits; break;
        case Category::Effect: map = &m_effects; break;
        case Category::Missile: map = &m_missiles; break;
    }
    auto it = map->find(id);
    if (it != map->end()) return &it->second;
    return nullptr;
}

} // namespace monx
