#include "utils.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <filesystem>

std::string getLogFileName(const std::string& baseName) {
    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d", localtime(&now));
    return baseName + "_" + timestamp + ".log";
}

void logMessage(const std::string& message, const std::string& logBaseName) {
    std::string logFile = getLogFileName(logBaseName);

    std::ofstream log(logFile, std::ios::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open log file: " << logFile << std::endl;
        return;
    }

    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    log << "[" << timestamp << "] " << message << std::endl;
    log.close();
}

Config loadConfig(const std::string& configPath) {
    if (!std::filesystem::exists(configPath)) {
        throw std::runtime_error("Failed to open config file: " + configPath);
    }

    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configPath);
    }

    nlohmann::json jsonConfig;
    file >> jsonConfig;

    Config config;
    config.serverIp = jsonConfig["server_ip"];
    config.serverPortStart = jsonConfig["server_port_start"];
    config.serverPortEnd = jsonConfig["server_port_end"];
    config.remoteServerIp = jsonConfig["remote_server_ip"];
    config.remoteServerPort = jsonConfig["remote_server_port"];
    config.logDirectory = jsonConfig["log_directory"];
    config.maxConnectionsPerIp = jsonConfig["max_connections_per_ip"];
    config.blockDurationMinutes = jsonConfig["block_duration_minutes"];

    logMessage("Configuration loaded: " + config.serverIp + ":" + std::to_string(config.serverPortStart) +
               " -> " + config.remoteServerIp + ":" + std::to_string(config.remoteServerPort), "system");

    return config;
}

bool isIpBlocked(const std::string& ip, std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, int maxConnections, int blockDurationSeconds) {
    auto now = std::chrono::steady_clock::now();

    for (auto it = ipMap.begin(); it != ipMap.end(); ) {
        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second.second).count() > blockDurationSeconds) {
            logMessage("Removing expired entry for IP: " + it->first, "proxy");
            it = ipMap.erase(it);
        } else {
            ++it;
        }
    }

    if (ipMap.find(ip) == ipMap.end()) {
        return false;
    }

    if (ipMap[ip].first >= maxConnections) {
        logMessage("IP blocked: " + ip, "error");
        return true;
    }

    return false;
}

void addIpConnection(const std::string& ip, std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>>& ipMap, int maxConnections, int blockDurationSeconds) {
    auto now = std::chrono::steady_clock::now();

    if (ipMap.find(ip) == ipMap.end()) {
        ipMap[ip] = {1, now};
        logMessage("New connection from IP: " + ip, "proxy");
    } else {
        if (ipMap[ip].first < maxConnections) {
            ipMap[ip].first++;
            ipMap[ip].second = now;
            logMessage("Incremented connection count for IP: " + ip, "proxy");
        }
    }
}