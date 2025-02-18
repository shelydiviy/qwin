#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <asio.hpp>

struct Config {
    std::string serverIp;
    int serverPortStart;
    int serverPortEnd;
    std::string masterServerIp;
    int masterServerPort;
    std::string serverName;
    std::string mapName;
    int players;
    int maxPlayers;
    int protocolVersion;
    std::vector<std::string> excludedIps;
    int maxConnectionsPerIp;
    int blockDurationMinutes;
};

// Функции для работы с IP-адресами
void addIpConnection(const std::string& ip, std::unordered_map<std::string, std::pair<int, time_t>>& ipMap,
                     const std::vector<std::string>& excludedIps, int maxConnections, int blockDurationSeconds);

bool isIpBlocked(const std::string& ip, const std::unordered_map<std::string, std::pair<int, time_t>>& ipMap,
                 const std::vector<std::string>& excludedIps, int maxConnections, int blockDurationSeconds);

// Функции для создания пакетов
size_t createMasterServerPacket(uint8_t* buffer, size_t bufferSize, const std::string& serverIp, int serverPort,
                               const std::string& serverName, const std::string& mapName, int players, int maxPlayers, int protocolVersion);

#endif // UTILS_H
