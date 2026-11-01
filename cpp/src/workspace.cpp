#include "monx/workspace.h"
#include "monx/monster_storage.h"
#include <iostream>

namespace monx {

Workspace& Workspace::instance() {
    static Workspace s_ws;
    return s_ws;
}

bool Workspace::open(const WorkspacePaths& paths, const std::string& engine_key) {
    namespace fs = std::filesystem;
    if (!fs::exists(paths.monsters)) return false;

    m_paths = paths;
    if (!engine_key.empty()) {
        m_profile = ProfileManager::get(engine_key);
    } else {
        m_profile = ProfileManager::get(ProfileManager::detect(paths.monsters));
    }

    m_docs.clear();
    m_summaries.clear();
    m_items.clear();

    // Load items if available
    if (!paths.items.empty() && fs::exists(paths.items)) {
        std::string items_xml = (fs::path(paths.items) / "items.xml").string();
        if (fs::exists(items_xml)) {
            FormatsManager::load_items_xml(items_xml, m_items);
        }
    }

    // Traverse monsters directory
    for (const auto& entry : fs::recursive_directory_iterator(paths.monsters)) {
        if (!entry.is_regular_file()) continue;

        std::string ext = entry.path().extension().string();
        if (ext != m_profile.file_extension) continue;

        MonsterDoc doc;
        std::string rel_path = fs::relative(entry.path(), paths.monsters).string();
        doc.file = rel_path;

        bool ok = false;
        if (m_profile.format == DocFormat::Xml) {
            ok = MonsterStorage::read_xml(entry.path().string(), doc, m_profile);
        } else {
            ok = MonsterStorage::read_lua(entry.path().string(), doc, m_profile);
        }

        if (ok) {
            MonsterSummary summary;
            summary.file = rel_path;
            summary.name = doc.name;
            summary.raceid = doc.raceid;
            summary.health = doc.health_max;
            summary.experience = doc.experience;
            summary.look = doc.look;

            auto lints = MonsterStorage::lint(doc, m_profile);
            for (const auto& l : lints) {
                if (l.severity == "error") summary.error_count++;
                else if (l.severity == "warning") summary.warning_count++;
                else if (l.severity == "silent") summary.silent_count++;
            }

            m_summaries.push_back(summary);
            m_docs[rel_path] = doc;
        }
    }

    m_open = true;
    return true;
}

void Workspace::close() {
    m_open = false;
    m_docs.clear();
    m_summaries.clear();
    m_items.clear();
}

std::optional<MonsterDoc> Workspace::get_monster(const std::string& file) const {
    auto it = m_docs.find(file);
    if (it != m_docs.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Workspace::save_monster(const MonsterDoc& doc) {
    namespace fs = std::filesystem;
    std::string full_path = (fs::path(m_paths.monsters) / doc.file).string();
    bool ok = false;
    if (m_profile.format == DocFormat::Xml) {
        ok = MonsterStorage::write_xml(full_path, doc, m_profile);
    } else {
        ok = MonsterStorage::write_lua(full_path, doc, m_profile);
    }

    if (ok) {
        m_docs[doc.file] = doc;
        for (auto& s : m_summaries) {
            if (s.file == doc.file) {
                s.name = doc.name;
                s.raceid = doc.raceid;
                s.health = doc.health_max;
                s.experience = doc.experience;
                s.look = doc.look;
                break;
            }
        }
    }
    return ok;
}

} // namespace monx
