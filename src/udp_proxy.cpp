#include "udp_proxy.h"
#include "utils.h"
#include <asio.hpp>
#include <unordered_map>
#include <chrono>

UdpProxy::UdpProxy(int startPortParam, int endPortParam, const std::string& remoteIpParam, int remotePortParam, const Config& configParam)
    : startPort(startPortParam), endPort(endPortParam), remoteServerIp(remoteIpParam), remoteServerPort(remotePortParam), config(configParam),
      clientEndpoint(), buffer() {}

void UdpProxy::startProxy() {
    logMessage("Starting UDP Proxy from ports " + std::to_string(startPort) + " to " + std::to_string(endPort) +
               " and forwarding to " + remoteServerIp + ":" + std::to_string(remoteServerPort), "proxy");

    try {
        for (int port = startPort; port <= endPort; ++port) {
            asio::ip::udp::socket sock(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
            sockets.push_back(std::move(sock));

            logMessage("Listening on port " + std::to_string(port), "proxy");

            sockets.back().async_receive_from(
                asio::buffer(buffer), clientEndpoint,
                [this, port](std::error_code ecOuter, std::size_t bytesReceivedOuter) { // Переименованные параметры
                    if (!ecOuter && bytesReceivedOuter > 0) {
                        handleIncomingPacket(sockets[port - startPort], bytesReceivedOuter);
                    } else if (ecOuter != asio::error::operation_aborted) {
                        logMessage("Error receiving packet on port " + std::to_string(port) + ": " + ecOuter.message(), "error");
                    }

                    if (!ecOuter) {
                        sockets[port - startPort].async_receive_from(
                            asio::buffer(buffer), clientEndpoint,
                            [this, port](std::error_code ecInner, std::size_t bytesReceivedInner) { // Переименованные параметры
                                if (!ecInner && bytesReceivedInner > 0) {
                                    handleIncomingPacket(sockets[port - startPort], bytesReceivedInner);
                                } else if (ecInner != asio::error::operation_aborted) {
                                    logMessage("Error receiving packet on port " + std::to_string(port) + ": " + ecInner.message(), "error");
                                }
                            });
                    }
                });
        }

        std::thread ioThread([this]() {
            ioContext.run();
        });

        ioThread.join();
    } catch (const std::exception& e) {
        logMessage("Error starting UDP Proxy: " + std::string(e.what()), "error");
    }
}

void UdpProxy::handleIncomingPacket(asio::ip::udp::socket& socket, std::size_t bytesReceived) {
    std::string clientIp = clientEndpoint.address().to_string();
    int clientPort = clientEndpoint.port();

    if (isIpBlocked(clientIp, ipMap, config.excludedIps, config.maxConnectionsPerIp, config.blockDurationMinutes * 60)) {
        logMessage("IP blocked: " + clientIp, "error");
        return;
    }

    addIpConnection(clientIp, ipMap, config.excludedIps, config.maxConnectionsPerIp, config.blockDurationMinutes * 60);

    logMessage("Received packet from " + clientIp + ":" + std::to_string(clientPort), "proxy");

    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::endpoint remoteEndpoint = *resolver.resolve(remoteServerIp, std::to_string(remoteServerPort)).begin();

        socket.async_send_to(
            asio::buffer(buffer, bytesReceived), remoteEndpoint,
            [clientIp](std::error_code ec, std::size_t bytesSent) { // Добавляем второй параметр
                if (ec) {
                    logMessage("Error sending packet to remote server for IP: " + clientIp + ": " + ec.message(), "error");
                }
            });
    } catch (const std::exception& e) {
        logMessage("Error handling packet for IP: " + clientIp + ": " + std::string(e.what()), "error");
    }
}