#include "monx/lua_monster_parser.h"
#include <sstream>
#include <regex>

namespace monx {

bool LuaMonsterParser::parse(const std::string& script, MonsterDoc& out_doc) {
    out_doc.raw_content = script;

    std::regex name_re(R"((?:mType:name|monster\.name)\s*\(?\s*["']([^"']+)["'])");
    std::smatch m;
    if (std::regex_search(script, m, name_re)) {
        out_doc.name = m[1].str();
    }

    std::regex desc_re(R"((?:mType:description|monster\.description)\s*\(?\s*["']([^"']+)["'])");
    if (std::regex_search(script, m, desc_re)) {
        out_doc.name_description = m[1].str();
    }

    std::regex exp_re(R"((?:mType:experience|monster\.experience)\s*\(?\s*([0-9]+))");
    if (std::regex_search(script, m, exp_re)) {
        out_doc.experience = std::stoul(m[1].str());
    }

    std::regex hp_re(R"((?:mType:health|monster\.health)\s*\(?\s*([0-9]+))");
    if (std::regex_search(script, m, hp_re)) {
        out_doc.health_max = std::stoul(m[1].str());
        out_doc.health_now = out_doc.health_max;
    }

    std::regex raceid_re(R"((?:mType:raceId|monster\.raceId|raceid)\s*\(?\s*([0-9]+))");
    if (std::regex_search(script, m, raceid_re)) {
        out_doc.raceid = std::stoul(m[1].str());
    }

    std::regex speed_re(R"((?:mType:speed|monster\.speed)\s*\(?\s*([0-9]+))");
    if (std::regex_search(script, m, speed_re)) {
        out_doc.speed = std::stoul(m[1].str());
    }

    std::regex mana_re(R"((?:mType:manacost|monster\.manacost)\s*\(?\s*([0-9]+))");
    if (std::regex_search(script, m, mana_re)) {
        out_doc.manacost = std::stoul(m[1].str());
    }

    // Parse voices
    std::regex voice_re(R"(\{\s*text\s*=\s*["']([^"']+)["'](?:,\s*yell\s*=\s*(true|false))?\s*\})");
    auto v_begin = std::sregex_iterator(script.begin(), script.end(), voice_re);
    auto v_end = std::sregex_iterator();
    for (std::sregex_iterator i = v_begin; i != v_end; ++i) {
        std::smatch match = *i;
        Voice v;
        v.sentence = match[1].str();
        v.yell = (match.size() > 2 && match[2].str() == "true") ? 1 : 0;
        out_doc.voices.push_back(v);
    }

    // Parse loot items
    std::regex loot_re(R"(\{\s*id\s*=\s*([0-9]+)\s*,\s*chance\s*=\s*([0-9]+)(?:,\s*maxCount\s*=\s*([0-9]+))?\s*\})");
    auto l_begin = std::sregex_iterator(script.begin(), script.end(), loot_re);
    auto l_end = std::sregex_iterator();
    for (std::sregex_iterator i = l_begin; i != l_end; ++i) {
        std::smatch match = *i;
        LootItem li;
        li.id = std::stoul(match[1].str());
        li.chance = std::stoul(match[2].str());
        li.countmax = (match.size() > 3 && !match[3].str().empty()) ? std::stoul(match[3].str()) : 1;
        out_doc.loot.push_back(li);
    }

    return true;
}

std::string LuaMonsterParser::serialize(const MonsterDoc& doc) {
    std::ostringstream ss;
    ss << "local mType = Game.createMonsterType(\"" << doc.name << "\")\n";
    ss << "local monster = {}\n\n";
    ss << "monster.description = \"" << doc.name_description << "\"\n";
    ss << "monster.experience = " << doc.experience << "\n";
    ss << "monster.health = " << doc.health_max << "\n";
    ss << "monster.maxHealth = " << doc.health_max << "\n";
    ss << "monster.raceId = " << doc.raceid << "\n";
    ss << "monster.speed = " << doc.speed << "\n";
    ss << "monster.manacost = " << doc.manacost << "\n\n";

    ss << "monster.outfit = {\n";
    ss << "\tlookType = " << doc.look.type.value_or(0) << ",\n";
    ss << "\tlookHead = " << doc.look.head << ",\n";
    ss << "\tlookBody = " << doc.look.body << ",\n";
    ss << "\tlookLegs = " << doc.look.legs << ",\n";
    ss << "\tlookFeet = " << doc.look.feet << ",\n";
    ss << "\tlookAddons = " << doc.look.addons << ",\n";
    ss << "\tlookMount = " << doc.look.mount << "\n";
    ss << "}\n\n";

    if (!doc.voices.empty()) {
        ss << "monster.voices = {\n\tinterval = 5000,\n\tchance = 10,\n";
        for (const auto& v : doc.voices) {
            ss << "\t{ text = \"" << v.sentence << "\", yell = " << (v.yell ? "true" : "false") << " },\n";
        }
        ss << "}\n\n";
    }

    if (!doc.loot.empty()) {
        ss << "monster.loot = {\n";
        for (const auto& l : doc.loot) {
            ss << "\t{ id = " << l.id << ", chance = " << l.chance << ", maxCount = " << l.countmax << " },\n";
        }
        ss << "}\n\n";
    }

    if (!doc.attacks.empty()) {
        ss << "monster.attacks = {\n";
        for (const auto& a : doc.attacks) {
            ss << "\t{ name = \"" << a.name << "\", interval = " << a.interval << ", chance = " << a.chance;
            if (a.min) ss << ", minDamage = " << *a.min;
            if (a.max) ss << ", maxDamage = " << *a.max;
            ss << " },\n";
        }
        ss << "}\n\n";
    }

    if (!doc.defenses.empty()) {
        ss << "monster.defenses = {\n";
        for (const auto& d : doc.defenses) {
            ss << "\t{ name = \"" << d.name << "\", interval = " << d.interval << ", chance = " << d.chance;
            if (d.min) ss << ", minDamage = " << *d.min;
            if (d.max) ss << ", maxDamage = " << *d.max;
            ss << " },\n";
        }
        ss << "}\n\n";
    }

    ss << "mType:register(monster)\n";
    return ss.str();
}

} // namespace monx
