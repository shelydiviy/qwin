#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <chrono>

struct Config {
    std::string serverIp;
    int serverPortStart;
    int serverPortEnd;
    std::string remoteServerIp;
    int remoteServerPort;
    std::string logDirectory;
    int maxConnectionsPerIp;
    int blockDurationMinutes;
};

Config loadConfig(const std::string& configPath);

void logMessage(const std::string& message, const std::string& logBaseName);

bool isIpBlocked(const std::string& ip, std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, int maxConnections, int blockDurationSeconds);

void addIpConnection(const std::string& ip, std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, int maxConnections, int blockDurationSeconds);

#endif // UTILS_H