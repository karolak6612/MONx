#include "monx/spr_reader.h"
#include <fstream>
#include <cstring>
#include <algorithm>

namespace monx {

std::optional<SprFileReader> SprFileReader::open(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return std::nullopt;

    std::streamsize size = file.tellg();
    if (size < 8) return std::nullopt;
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), size)) return std::nullopt;

    SprFileReader reader;
    reader.m_data = std::move(data);
    reader.m_info.file_size = size;

    std::memcpy(&reader.m_info.signature, reader.m_data.data(), 4);

    uint16_t count16 = 0;
    std::memcpy(&count16, reader.m_data.data() + 4, 2);

    uint32_t count32 = 0;
    std::memcpy(&count32, reader.m_data.data() + 4, 4);

    // Layout heuristic: check if count32 table fits within file size
    if (8 + (uint64_t)count32 * 4 <= (uint64_t)size) {
        reader.m_info.extended = true;
        reader.m_info.sprite_count = count32;
        reader.m_header_size = 8;
    } else {
        reader.m_info.extended = false;
        reader.m_info.sprite_count = count16;
        reader.m_header_size = 6;
    }

    return reader;
}

SpriteData SprFileReader::sprite_data(uint32_t id) const {
    SpriteData empty{id, true, {}};
    if (id == 0 || id > m_info.sprite_count) return empty;

    size_t off = m_header_size + (id - 1) * 4;
    if (off + 4 > m_data.size()) return empty;

    uint32_t addr = 0;
    std::memcpy(&addr, m_data.data() + off, 4);
    if (addr == 0 || addr + 5 > m_data.size()) return empty;

    uint16_t length = 0;
    std::memcpy(&length, m_data.data() + addr + 3, 2);
    if (length == 0 || addr + 5 + length > m_data.size()) return empty;

    SpriteData result;
    result.id = id;
    result.is_empty = false;
    result.compressed_pixels.assign(m_data.begin() + addr + 5, m_data.begin() + addr + 5 + length);
    return result;
}

std::vector<uint8_t> SprFileReader::decompress_to_rgba(const std::vector<uint8_t>& compressed, bool transparent) {
    std::vector<uint8_t> pixels(SPRITE_DATA_SIZE, 0);
    size_t write_pos = 0;
    size_t read_pos = 0;
    size_t channels = transparent ? 4 : 3;

    while (read_pos + 4 <= compressed.size() && write_pos < SPRITE_DATA_SIZE) {
        uint16_t transparent_count = 0;
        std::memcpy(&transparent_count, compressed.data() + read_pos, 2);
        read_pos += 2;

        uint16_t colored_count = 0;
        std::memcpy(&colored_count, compressed.data() + read_pos, 2);
        read_pos += 2;

        write_pos += transparent_count * 4;

        size_t bytes_needed = colored_count * channels;
        if (read_pos + bytes_needed > compressed.size()) break;

        for (size_t i = 0; i < colored_count && write_pos + 4 <= SPRITE_DATA_SIZE; ++i) {
            uint8_t r = compressed[read_pos++];
            uint8_t g = compressed[read_pos++];
            uint8_t b = compressed[read_pos++];
            uint8_t a = transparent ? compressed[read_pos++] : 255;

            pixels[write_pos++] = r;
            pixels[write_pos++] = g;
            pixels[write_pos++] = b;
            pixels[write_pos++] = a;
        }
    }
    return pixels;
}

} // namespace monx
