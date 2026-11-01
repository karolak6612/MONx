#pragma once

#include "monx/monster_model.h"
#include "monx/engine_profile.h"
#include "monx/formats_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace monx {

struct WorkspacePaths {
    std::string monsters;
    std::string items;
    std::string client;
    std::string spells;
};

class Workspace {
public:
    static Workspace& instance();

    bool open(const WorkspacePaths& paths, const std::string& engine_key = "");
    void close();

    bool is_open() const { return m_open; }
    const WorkspacePaths& paths() const { return m_paths; }
    const EngineProfile& profile() const { return m_profile; }

    const std::vector<MonsterSummary>& monsters() const { return m_summaries; }
    std::optional<MonsterDoc> get_monster(const std::string& file) const;
    bool save_monster(const MonsterDoc& doc);

    const std::unordered_map<uint32_t, ItemInfo>& items() const { return m_items; }

private:
    Workspace() = default;

    bool m_open = false;
    WorkspacePaths m_paths;
    EngineProfile m_profile;

    std::vector<MonsterSummary> m_summaries;
    std::unordered_map<std::string, MonsterDoc> m_docs;
    std::unordered_map<uint32_t, ItemInfo> m_items;
};

} // namespace monx
