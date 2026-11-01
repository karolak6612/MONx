#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <wx/bitmap.h>

namespace monx {

enum class Category {
    Item,
    Outfit,
    Effect,
    Missile
};

struct ItemInfo {
    uint32_t server_id = 0;
    uint32_t client_id = 0;
    std::string name;
    bool is_container = false;
    bool is_fluid_container = false;
    bool pickupable = true;
    uint32_t weight = 0;
};

class FormatsManager {
public:
    static bool load_items_xml(const std::string& path, std::unordered_map<uint32_t, ItemInfo>& out_items);
    static bool load_items_otb(const std::string& path, std::unordered_map<uint32_t, uint32_t>& out_server_to_client);

    // Dummy/placeholder rendering generator for outfits and items
    static wxBitmap render_thing(Category cat, uint32_t id, uint32_t cell = 64);
    static wxBitmap render_look(const class Look& look, uint32_t dir = 2, uint32_t cell = 64);
};

} // namespace monx
