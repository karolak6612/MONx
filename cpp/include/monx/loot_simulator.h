#pragma once

#include "monx/monster_model.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace monx {

struct LootSimParams {
    uint32_t kills_per_session = 1000;
    double loot_rate = 1.0;
    uint32_t sessions = 1;
    uint32_t seed = 42;
};

struct ItemSimStat {
    uint32_t id = 0;
    std::string name;
    uint32_t total_dropped = 0;
    uint32_t kills_with_drop = 0;
    double expected_rate = 0.0;
};

struct LootSimResult {
    uint32_t total_kills = 0;
    std::vector<ItemSimStat> items;
};

class LootSimulator {
public:
    static LootSimResult run(const std::vector<LootItem>& loot, const LootSimParams& params);
};

} // namespace monx
