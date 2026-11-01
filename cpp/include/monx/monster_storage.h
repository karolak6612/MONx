#pragma once

#include "monx/monster_model.h"
#include "monx/engine_profile.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace monx {

class MonsterStorage {
public:
    static bool read_xml(const std::string& path, MonsterDoc& out_doc, const EngineProfile& profile);
    static bool write_xml(const std::string& path, const MonsterDoc& doc, const EngineProfile& profile);

    static bool read_lua(const std::string& path, MonsterDoc& out_doc, const EngineProfile& profile);
    static bool write_lua(const std::string& path, const MonsterDoc& doc, const EngineProfile& profile);

    static std::vector<Lint> lint(const MonsterDoc& doc, const EngineProfile& profile);
};

} // namespace monx
