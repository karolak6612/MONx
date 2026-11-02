#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>

namespace monx {

constexpr size_t SPRITE_SIZE = 32;
constexpr size_t SPRITE_PIXELS = SPRITE_SIZE * SPRITE_SIZE;
constexpr size_t SPRITE_DATA_SIZE = SPRITE_PIXELS * 4;

struct SprInfo {
    uint32_t signature = 0;
    uint32_t sprite_count = 0;
    bool extended = false;
    uint64_t file_size = 0;
};

struct SpriteData {
    uint32_t id = 0;
    bool is_empty = true;
    std::vector<uint8_t> compressed_pixels;
};

class SprFileReader {
public:
    static std::optional<SprFileReader> open(const std::string& path);

    const SprInfo& get_info() const { return m_info; }
    SpriteData sprite_data(uint32_t id) const;

    static std::vector<uint8_t> decompress_to_rgba(const std::vector<uint8_t>& compressed, bool transparent = true);

private:
    std::vector<uint8_t> m_data;
    SprInfo m_info;
    size_t m_header_size = 0;
};

} // namespace monx
