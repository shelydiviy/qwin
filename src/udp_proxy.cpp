#include "udp_proxy.h"
#include "utils.h"
#include <asio.hpp>
#include <unordered_map>
#include <memory>
#include <chrono>

// Конструктор с переименованными параметрами
UdpProxy::UdpProxy(int startPortNum, int endPortNum, const std::string& remoteIpAddr, int remotePortNum, const Config& configData)
    : startPort(startPortNum), endPort(endPortNum), remoteServerIp(remoteIpAddr), remoteServerPort(remotePortNum), config(configData),
      clientEndpoint(), buffer() {}

void UdpProxy::startProxy() {
    logMessage("Запуск UDP Proxy с портов " + std::to_string(startPort) + " до " + std::to_string(endPort) +
               " и перенаправление на " + remoteServerIp + ":" + std::to_string(remoteServerPort), "proxy");

    try {
        for (int port = startPort; port <= endPort; ++port) {
            asio::ip::udp::socket sock(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
            sockets.push_back(std::move(sock));

            logMessage("Начало прослушивания порта " + std::to_string(port), "proxy");

            sockets.back().async_receive_from(
                asio::buffer(buffer), clientEndpoint,
                [this, port](std::error_code ecOuter, std::size_t bytesReceivedOuter) {
                    if (!ecOuter && bytesReceivedOuter > 0) {
                        handleIncomingPacket(sockets[port - startPort], bytesReceivedOuter);
                    } else if (ecOuter != asio::error::operation_aborted) {
                        logMessage("Ошибка приёма пакета на порту " + std::to_string(port) + ": " + ecOuter.message(), "error");
                    }

                    if (!ecOuter) {
                        sockets[port - startPort].async_receive_from(
                            asio::buffer(buffer), clientEndpoint,
                            [this, port](std::error_code ecInner, [[maybe_unused]] std::size_t bytesReceivedInner) {
                                if (!ecInner) {
                                    handleIncomingPacket(sockets[port - startPort], bytesReceivedInner);
                                } else if (ecInner != asio::error::operation_aborted) {
                                    logMessage("Ошибка приёма пакета на порту " + std::to_string(port) + ": " + ecInner.message(), "error");
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
        logMessage("Ошибка запуска UDP Proxy: " + std::string(e.what()), "error");
    }
}

void UdpProxy::handleIncomingPacket(asio::ip::udp::socket& socket, std::size_t bytesReceived) {
    std::string clientIp = clientEndpoint.address().to_string();
    int clientPort = clientEndpoint.port();

    if (isIpBlocked(clientIp, ipMap, config.excludedIps, config.maxConnectionsPerIp, config.blockDurationMinutes * 60)) {
        logMessage("IP заблокирован: " + clientIp, "error");
        return;
    }

    addIpConnection(clientIp, ipMap, config.excludedIps, config.maxConnectionsPerIp);

    logMessage("Получен пакет от клиента " + clientIp + ":" + std::to_string(clientPort), "proxy");

    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::endpoint remoteEndpoint = *resolver.resolve(remoteServerIp, std::to_string(remoteServerPort)).begin();

        socket.async_send_to(
            asio::buffer(buffer, bytesReceived), remoteEndpoint,
            [clientIp](std::error_code ec, [[maybe_unused]] std::size_t /*bytesSent*/) { // Убираем unused parameter
                if (ec) {
                    logMessage("Ошибка отправки пакета клиенту " + clientIp + ": " + ec.message(), "error");
                }
            });
    } catch (const std::exception& e) {
        logMessage("Ошибка обработки пакета от IP: " + clientIp + ": " + std::string(e.what()), "error");
    }
}
