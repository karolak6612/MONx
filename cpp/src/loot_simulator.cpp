#include "monx/loot_simulator.h"
#include <random>

namespace monx {

LootSimResult LootSimulator::run(const std::vector<LootItem>& loot, const LootSimParams& params) {
    LootSimResult result;
    result.total_kills = params.kills_per_session * params.sessions;

    std::mt19937 rng(params.seed);
    std::uniform_real_distribution<double> dist(0.0, 100000.0);

    std::unordered_map<uint32_t, ItemSimStat> stats;

    for (const auto& li : loot) {
        ItemSimStat s;
        s.id = li.id;
        s.name = (li.comment.empty()) ? ("Item #" + std::to_string(li.id)) : li.comment;
        s.expected_rate = (double)li.chance / 100000.0;
        stats[li.id] = s;
    }

    for (uint32_t k = 0; k < result.total_kills; ++k) {
        for (const auto& li : loot) {
            double roll = dist(rng);
            if (roll <= (double)li.chance * params.loot_rate) {
                auto& s = stats[li.id];
                s.kills_with_drop++;

                uint32_t count = 1;
                if (li.countmax > 1) {
                    std::uniform_int_distribution<uint32_t> cdist(1, li.countmax);
                    count = cdist(rng);
                }
                s.total_dropped += count;
            }
        }
    }

    for (auto& pair : stats) {
        result.items.push_back(pair.second);
    }
    return result;
}

} // namespace monx
