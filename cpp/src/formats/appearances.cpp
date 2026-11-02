#include "monx/appearances.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace monx {

std::optional<ModernBundle> ModernBundle::open(const std::string& folder_path) {
    namespace fs = std::filesystem;
    if (!fs::exists(folder_path)) return std::nullopt;

    fs::path catalog_path = fs::path(folder_path) / "catalog-content.json";
    if (!fs::exists(catalog_path)) return std::nullopt;

    std::ifstream f(catalog_path);
    if (!f) return std::nullopt;

    ModernBundle bundle;
    try {
        nlohmann::json j;
        f >> j;
        for (const auto& item : j) {
            if (item.contains("type") && item["type"] == "sheets" && item.contains("file")) {
                AppearanceSpriteSheet sheet;
                sheet.file_name = item["file"].get<std::string>();
                bundle.m_sheets.push_back(sheet);
            }
        }
        bundle.m_valid = true;
    } catch (...) {
        return std::nullopt;
    }

    return bundle;
}

} // namespace monx
