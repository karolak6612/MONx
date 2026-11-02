#include "monx/otb_reader.h"
#include <fstream>
#include <cstring>

namespace monx {

constexpr uint8_t NODE_START = 0xFE;
constexpr uint8_t NODE_END = 0xFF;
constexpr uint8_t NODE_ESCAPE = 0xFD;

constexpr uint8_t ITEM_ATTR_SERVERID = 0x10;
constexpr uint8_t ITEM_ATTR_CLIENTID = 0x11;

std::optional<OtbReader> OtbReader::load(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return std::nullopt;

    size_t size = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> buf(size);
    if (!f.read(reinterpret_cast<char*>(buf.data()), size)) return std::nullopt;

    OtbReader reader;
    size_t pos = 4; // skip 4-byte signature

    auto read_u8 = [&]() -> std::optional<uint8_t> {
        if (pos >= size) return std::nullopt;
        return buf[pos++];
    };

    while (pos < size) {
        auto b = read_u8();
        if (!b) break;
        if (*b == NODE_START) {
            auto type_b = read_u8();
            if (!type_b) break;

            // Flags
            pos += 4;

            // Read props
            std::vector<uint8_t> props;
            while (pos < size) {
                uint8_t pb = buf[pos];
                if (pb == NODE_START || pb == NODE_END) break;
                if (pb == NODE_ESCAPE) {
                    pos++;
                    if (pos < size) props.push_back(buf[pos++]);
                } else {
                    props.push_back(buf[pos++]);
                }
            }

            // Parse attributes in props
            size_t ppos = 0;
            uint32_t sid = 0, cid = 0;
            while (ppos + 3 <= props.size()) {
                uint8_t attr = props[ppos++];
                uint16_t len = props[ppos] | (props[ppos + 1] << 8);
                ppos += 2;
                if (ppos + len > props.size()) break;

                if (attr == ITEM_ATTR_SERVERID && len == 2) {
                    sid = props[ppos] | (props[ppos + 1] << 8);
                } else if (attr == ITEM_ATTR_CLIENTID && len == 2) {
                    cid = props[ppos] | (props[ppos + 1] << 8);
                }
                ppos += len;
            }

            if (sid > 0 && cid > 0) {
                reader.m_server_to_client[sid] = cid;
                reader.m_client_to_server[cid] = sid;
            }
        }
    }

    return reader;
}

std::optional<uint32_t> OtbReader::client_id(uint32_t server_id) const {
    auto it = m_server_to_client.find(server_id);
    if (it != m_server_to_client.end()) return it->second;
    return std::nullopt;
}

std::optional<uint32_t> OtbReader::server_id(uint32_t client_id) const {
    auto it = m_client_to_server.find(client_id);
    if (it != m_client_to_server.end()) return it->second;
    return std::nullopt;
}

} // namespace monx
