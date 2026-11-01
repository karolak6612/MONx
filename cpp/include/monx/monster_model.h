#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace monx {

struct Look {
    std::string mode = "type"; // "type" or "typeex"
    std::optional<uint32_t> type;
    std::optional<uint32_t> typeex;
    uint32_t head = 0;
    uint32_t body = 0;
    uint32_t legs = 0;
    uint32_t feet = 0;
    uint32_t addons = 0;
    uint32_t mount = 0;
    uint32_t corpse = 0;
    uint32_t corpseactionid = 0;
};

struct Voice {
    std::string sentence;
    uint32_t yell = 0;
};

struct LootItem {
    uint32_t id = 0;
    uint32_t countmax = 1;
    uint32_t chance = 1000;
    uint32_t action_id = 0;
    std::string comment;
    std::vector<LootItem> subitems;
};

struct Spell {
    std::string name;
    uint32_t interval = 2000;
    uint32_t chance = 100;
    std::optional<int32_t> min;
    std::optional<int32_t> max;
    uint32_t range = 1;
    uint32_t radius = 0;
    uint32_t target = 0;
    std::string shoot_effect;
    std::string area_effect;
};

struct Summon {
    std::string name;
    uint32_t interval = 2000;
    uint32_t chance = 100;
    uint32_t max = 1;
    bool force = false;
};

struct TargetStrategy {
    uint32_t nearest = 0;
    uint32_t health = 0;
    uint32_t damage = 0;
    uint32_t random = 0;
};

struct MonsterDoc {
    std::string file; // Relative path, e.g. "monsters/demon.xml"
    std::string name;
    std::string name_description;
    uint32_t raceid = 0;
    uint32_t experience = 0;
    uint32_t speed = 200;
    uint32_t manacost = 0;
    uint32_t health_now = 100;
    uint32_t health_max = 100;
    Look look;
    TargetStrategy target_strategy;
    std::unordered_map<std::string, std::string> flags;
    std::unordered_map<std::string, int32_t> elements;
    std::unordered_map<std::string, bool> immunities;
    std::vector<Voice> voices;
    std::vector<LootItem> loot;
    std::vector<Spell> attacks;
    std::vector<Spell> defenses;
    std::vector<Summon> summons;

    // Raw XML / Lua preservation
    std::string raw_content;
};

struct MonsterSummary {
    std::string file;
    std::string name;
    uint32_t raceid = 0;
    uint32_t health = 0;
    uint32_t experience = 0;
    Look look;
    uint32_t error_count = 0;
    uint32_t warning_count = 0;
    uint32_t silent_count = 0;
};

struct Lint {
    std::string file;
    std::string code;
    std::string severity; // "error", "warning", "silent"
    std::string message;
    std::string field;
};

} // namespace monx
