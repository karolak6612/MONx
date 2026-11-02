#include "monx/dat_reader.h"
#include <fstream>
#include <cstring>

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

    // Initialize map structures
    for (uint32_t i = 100; i <= item_count; ++i) {
        ThingType t;
        t.category = Category::Item;
        t.id = i;
        reader.m_items[i] = t;
    }
    for (uint32_t i = 1; i <= outfit_count; ++i) {
        ThingType t;
        t.category = Category::Outfit;
        t.id = i;
        reader.m_outfits[i] = t;
    }
    for (uint32_t i = 1; i <= effect_count; ++i) {
        ThingType t;
        t.category = Category::Effect;
        t.id = i;
        reader.m_effects[i] = t;
    }
    for (uint32_t i = 1; i <= missile_count; ++i) {
        ThingType t;
        t.category = Category::Missile;
        t.id = i;
        reader.m_missiles[i] = t;
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
