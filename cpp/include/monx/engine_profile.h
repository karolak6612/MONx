#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

namespace monx {

enum class EngineId {
    Tfs,
    Ironcore,
    Tvp,
    Nostalrius,
    Canary,
    Crystal,
    BlackTek
};

enum class DocFormat {
    Xml,
    Lua
};

struct EngineProfile {
    EngineId id;
    std::string key;
    std::string name;
    DocFormat format;
    std::string file_extension;
    std::string raceid_attr;
    bool has_sound_element;
    bool has_voices;
    bool has_loot;
    bool has_attacks;
    bool has_defenses;
    bool has_elements;
    bool has_immunities;
    bool has_summons;
    bool has_script_element;
    bool has_bestiary;
    bool has_pacifist;
};

class ProfileManager {
public:
    static const EngineProfile& get(EngineId id);
    static const EngineProfile& get(std::string_view key);
    static const std::vector<EngineProfile>& all();
    static EngineId detect(const std::string& monsters_dir);
};

} // namespace monx
