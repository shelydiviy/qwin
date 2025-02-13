#ifndef UDP_PROXY_H
#define UDP_PROXY_H

#include <string>
#include <asio.hpp>
#include <unordered_map>
#include <chrono>
#include <vector>

class UdpProxy {
public:
    UdpProxy(int startPort, int endPort, const std::string& remoteIp, int remotePort);
    void startProxy();

private:
    int startPort;
    int endPort;
    std::string remoteServerIp;
    int remoteServerPort;

    asio::io_context ioContext; // Единый io_context для всех портов
    std::vector<asio::ip::udp::socket> sockets; // Вектор сокетов
    std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>> ipMap;

    asio::ip::udp::endpoint clientEndpoint; // Endpoint для клиента
    std::array<char, 1024> buffer; // Буфер для данных

    void handleIncomingPacket(asio::ip::udp::socket& socket, std::size_t bytesReceived);
};

#endif // UDP_PROXY_H