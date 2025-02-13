#include "udp_proxy.h"
#include "utils.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h> // Добавляем эту строку

UdpProxy::UdpProxy(int startPort, int endPort, const std::string& remoteIp, int remotePort)
    : startPort(startPort), endPort(endPort), remoteServerIp(remoteIp), remoteServerPort(remotePort) {}

void UdpProxy::startProxy() {
    logMessage("Starting UDP Proxy from ports " + std::to_string(startPort) + " to " + std::to_string(endPort) +
               " and forwarding to " + remoteServerIp + ":" + std::to_string(remoteServerPort), "proxy");

    std::unordered_map<std::string, std::pair<int, std::chrono::steady_clock::time_point>> ipMap;

    for (int port = startPort; port <= endPort; ++port) {
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            logMessage("Failed to create socket for port " + std::to_string(port), "error");
            continue;
        }

        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            logMessage("Failed to bind to port " + std::to_string(port), "error");
            close(sockfd); // Используется здесь
            continue;
        }

        logMessage("Listening on port " + std::to_string(port), "proxy");

        while (true) {
            char buffer[1024];
            sockaddr_in clientAddr {};
            socklen_t clientAddrLen = sizeof(clientAddr);

            ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientAddrLen);
            if (bytesReceived <= 0) {
                continue;
            }

            char clientIp[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);

            if (isIpBlocked(clientIp, ipMap, 10, 600)) {
                logMessage("IP blocked: " + std::string(clientIp), "error");
                continue;
            }

            addIpConnection(clientIp, ipMap, 10, 600);

            sockaddr_in remoteAddr {};
            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_port = htons(remoteServerPort);
            inet_pton(AF_INET, remoteServerIp.c_str(), &(remoteAddr.sin_addr));

            sendto(sockfd, buffer, bytesReceived, 0, (sockaddr*)&remoteAddr, sizeof(remoteAddr));
        }
    }
}