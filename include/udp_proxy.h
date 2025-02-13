#ifndef UDP_PROXY_H
#define UDP_PROXY_H

#include <string>
#include <asio.hpp>
#include <unordered_map>
#include <chrono>
#include <vector>
#include "utils.h" // Для Config

class UdpProxy {
public:
    UdpProxy(int startPortParam, int endPortParam, const std::string& remoteIpParam, int remotePortParam, const Config& configParam);
    void startProxy();

private:
    int startPort;
    int endPort;
    std::string remoteServerIp;
    int remoteServerPort;
    Config config; // Храним конфигурацию

    asio::io_context ioContext;
    std::vector<asio::ip::udp::socket> sockets;
    std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>> ipMap;

    asio::ip::udp::endpoint clientEndpoint;
    std::array<char, 1024> buffer;

    void handleIncomingPacket(asio::ip::udp::socket& socket, std::size_t bytesReceived);
};

#endif // UDP_PROXY_H