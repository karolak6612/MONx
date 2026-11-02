#include "monx/full_linter.h"

namespace monx {

std::vector<Lint> FullLinter::run(const MonsterDoc& doc, const EngineProfile& profile) {
    std::vector<Lint> lints;

    if (doc.name.empty()) {
        lints.push_back({doc.file, "name.empty", "error", "Monster name cannot be empty", "name"});
    }
    if (doc.health_max == 0) {
        lints.push_back({doc.file, "health.zero", "error", "Max health must be greater than zero", "health"});
    }
    if (doc.health_now > doc.health_max) {
        lints.push_back({doc.file, "health.now-exceeds-max", "warning", "Initial health exceeds max health", "health"});
    }
    if (doc.look.mode == "type" && !doc.look.type.has_value()) {
        lints.push_back({doc.file, "look.missing-type", "warning", "Look has no type specified", "look"});
    }
    if (doc.speed == 0) {
        lints.push_back({doc.file, "speed.zero", "warning", "Speed is zero; creature cannot move", "speed"});
    }
    if (doc.attacks.empty() && !doc.flags.count("hostile")) {
        lints.push_back({doc.file, "attacks.empty", "silent", "No attacks defined for creature", "attacks"});
    }

    for (const auto& atk : doc.attacks) {
        if (atk.interval < 500) {
            lints.push_back({doc.file, "attack.interval-too-fast", "warning", "Attack interval is under 500ms", "attacks"});
        }
        if (atk.chance > 100) {
            lints.push_back({doc.file, "attack.chance-over-100", "error", "Attack chance cannot exceed 100%", "attacks"});
        }
    }

    for (const auto& li : doc.loot) {
        if (li.chance == 0) {
            lints.push_back({doc.file, "loot.chance-zero", "silent", "Loot item has 0 chance to drop", "loot"});
        }
        if (li.chance > 100000) {
            lints.push_back({doc.file, "loot.chance-over-max", "error", "Loot chance exceeds 100,000 ppm", "loot"});
        }
        if (li.countmax > 100) {
            lints.push_back({doc.file, "loot.countmax-over-100", "error", "Item countmax exceeds 100", "loot"});
        }
    }

    return lints;
}

} // namespace monx
