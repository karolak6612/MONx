#pragma once

#include "monx/monster_model.h"
#include "monx/engine_profile.h"
#include <string>
#include <vector>

namespace monx {

class FullLinter {
public:
    static std::vector<Lint> run(const MonsterDoc& doc, const EngineProfile& profile);
};

} // namespace monx
