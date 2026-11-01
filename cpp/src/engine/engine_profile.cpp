#include "monx/engine_profile.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace monx {

static const std::vector<EngineProfile> PROFILES = {
    {
        .id = EngineId::Tfs,
        .key = "tfs",
        .name = "TheForgottenServer 1.x",
        .format = DocFormat::Xml,
        .file_extension = ".xml",
        .raceid_attr = "raceId",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = true,
        .has_bestiary = true,
        .has_pacifist = false,
    },
    {
        .id = EngineId::Ironcore,
        .key = "ironcore",
        .name = "Ironcore",
        .format = DocFormat::Xml,
        .file_extension = ".xml",
        .raceid_attr = "raceid",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = false,
        .has_pacifist = true,
    },
    {
        .id = EngineId::Tvp,
        .key = "tvp",
        .name = "TheVioletProject",
        .format = DocFormat::Xml,
        .file_extension = ".xml",
        .raceid_attr = "raceid",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = false,
        .has_pacifist = false,
    },
    {
        .id = EngineId::Nostalrius,
        .key = "nostalrius",
        .name = "Nostalrius",
        .format = DocFormat::Xml,
        .file_extension = ".xml",
        .raceid_attr = "raceid",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = false,
        .has_pacifist = false,
    },
    {
        .id = EngineId::Canary,
        .key = "canary",
        .name = "Canary / OTServBR",
        .format = DocFormat::Lua,
        .file_extension = ".lua",
        .raceid_attr = "raceId",
        .has_sound_element = true,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = true,
        .has_pacifist = false,
    },
    {
        .id = EngineId::Crystal,
        .key = "crystal",
        .name = "CrystalServer",
        .format = DocFormat::Lua,
        .file_extension = ".lua",
        .raceid_attr = "raceId",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = true,
        .has_pacifist = false,
    },
    {
        .id = EngineId::BlackTek,
        .key = "blacktek",
        .name = "BlackTek",
        .format = DocFormat::Lua,
        .file_extension = ".lua",
        .raceid_attr = "raceid",
        .has_sound_element = false,
        .has_voices = true,
        .has_loot = true,
        .has_attacks = true,
        .has_defenses = true,
        .has_elements = true,
        .has_immunities = true,
        .has_summons = true,
        .has_script_element = false,
        .has_bestiary = false,
        .has_pacifist = false,
    }
};

const EngineProfile& ProfileManager::get(EngineId id) {
    for (const auto& p : PROFILES) {
        if (p.id == id) return p;
    }
    return PROFILES[0];
}

const EngineProfile& ProfileManager::get(std::string_view key) {
    for (const auto& p : PROFILES) {
        if (p.key == key) return p;
    }
    return PROFILES[0];
}

const std::vector<EngineProfile>& ProfileManager::all() {
    return PROFILES;
}

EngineId ProfileManager::detect(const std::string& monsters_dir) {
    namespace fs = std::filesystem;
    if (!fs::exists(monsters_dir)) return EngineId::Tfs;

    // Check for Lua monsters
    for (const auto& entry : fs::recursive_directory_iterator(monsters_dir)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".lua") {
                return EngineId::Canary;
            }
        }
    }
    return EngineId::Tfs;
}

} // namespace monx
