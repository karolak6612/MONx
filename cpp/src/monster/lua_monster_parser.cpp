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

    ss << "mType:register(monster)\n";
    return ss.str();
}

} // namespace monx
