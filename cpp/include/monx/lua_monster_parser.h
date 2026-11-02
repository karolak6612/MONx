#pragma once

#include "monx/monster_model.h"
#include <string>
#include <vector>

namespace monx {

class LuaMonsterParser {
public:
    static bool parse(const std::string& script, MonsterDoc& out_doc);
    static std::string serialize(const MonsterDoc& doc);
};

} // namespace monx
