#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>

namespace monx {

class OtbReader {
public:
    static std::optional<OtbReader> load(const std::string& path);

    std::optional<uint32_t> client_id(uint32_t server_id) const;
    std::optional<uint32_t> server_id(uint32_t client_id) const;
    const std::unordered_map<uint32_t, uint32_t>& server_to_client() const { return m_server_to_client; }

private:
    std::unordered_map<uint32_t, uint32_t> m_server_to_client;
    std::unordered_map<uint32_t, uint32_t> m_client_to_server;
};

} // namespace monx
