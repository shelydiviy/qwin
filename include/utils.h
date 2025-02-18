#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <chrono>
#include <atomic>

struct Config {
    std::string serverIp;
    int serverPortStart;
    int serverPortEnd;
    std::string remoteServerIp;
    int remoteServerPort;
    std::string logDirectory;
    int maxConnectionsPerIp;
    int blockDurationMinutes;
    std::vector<std::string> excludedIps;
};

std::string getLogFileName(const std::string& baseName);
void logMessage(const std::string& message, const std::string& logBaseName);
Config loadConfig(const std::string& configPath);

bool isIpBlocked(const std::string& ip, 
                 std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, 
                 const std::vector<std::string>& excludedIps, 
                 int maxConnections, 
                 int blockDurationSeconds);

void addIpConnection(const std::string& ip, 
                     std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, 
                     const std::vector<std::string>& excludedIps, 
                     int maxConnections);

extern std::atomic<int> totalConnections;
extern std::atomic<int> blockedConnections;

#endif // UTILS_H