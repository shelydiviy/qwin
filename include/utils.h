#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <chrono>
#include <atomic>

struct Config {
    std::string serverIp; // IP-адрес сервера
    int serverPortStart;  // Начальный порт для серверов
    int serverPortEnd;    // Конечный порт для серверов
    std::string remoteServerIp; // IP удалённого сервера
    int remoteServerPort;      // Порт удалённого сервера
    std::string logDirectory;  // Директория для логов
    int maxConnectionsPerIp;   // Максимальное количество подключений с одного IP
    int blockDurationMinutes;  // Время блокировки IP (в минутах)
    std::vector<std::string> excludedIps; // Список исключённых IP
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
                     int maxConnections, 
                     int blockDurationSeconds);

extern std::atomic<int> totalConnections;
extern std::atomic<int> blockedConnections;

#endif // UTILS_H