#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

namespace monx {

struct AppearanceSpriteSheet {
    uint32_t sheet_id = 0;
    std::string file_name;
};

class ModernBundle {
public:
    static std::optional<ModernBundle> open(const std::string& folder_path);

    bool is_valid() const { return m_valid; }
    uint32_t sheet_count() const { return (uint32_t)m_sheets.size(); }

private:
    bool m_valid = false;
    std::vector<AppearanceSpriteSheet> m_sheets;
};

} // namespace monx
