#include "udp_proxy.h"
#include "utils.h"
#include <asio.hpp>
#include <unordered_map>
#include <chrono>

UdpProxy::UdpProxy(int startPort, int endPort, const std::string& remoteIp, int remotePort)
    : startPort(startPort), endPort(endPort), remoteServerIp(remoteIp), remoteServerPort(remotePort),
      clientEndpoint(), buffer() {}

void UdpProxy::startProxy() {
    logMessage("Starting UDP Proxy from ports " + std::to_string(startPort) + " to " + std::to_string(endPort) +
               " and forwarding to " + remoteServerIp + ":" + std::to_string(remoteServerPort), "proxy");

    try {
        // Создание сокетов для каждого порта
        for (int port = startPort; port <= endPort; ++port) {
            asio::ip::udp::socket sock(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
            sockets.push_back(std::move(sock)); // Добавляем сокет в вектор

            logMessage("Listening on port " + std::to_string(port), "proxy");

            // Начинаем асинхронный приём пакетов
            sockets.back().async_receive_from(
                asio::buffer(buffer), clientEndpoint,
                [this, port](std::error_code ec, std::size_t bytesReceived) {
                    if (!ec && bytesReceived > 0) {
                        handleIncomingPacket(sockets[port - startPort], bytesReceived); // Обрабатываем пакет
                    } else if (ec != asio::error::operation_aborted) { // Игнорируем Operation Aborted
                        logMessage("Error receiving packet on port " + std::to_string(port) + ": " + ec.message(), "error");
                    }

                    // После обработки снова начинаем слушать
                    if (!ec) {
                        sockets[port - startPort].async_receive_from(
                            asio::buffer(buffer), clientEndpoint,
                            [this, port](std::error_code ec, std::size_t bytesReceived) {
                                if (!ec && bytesReceived > 0) {
                                    handleIncomingPacket(sockets[port - startPort], bytesReceived);
                                } else if (ec != asio::error::operation_aborted) {
                                    logMessage("Error receiving packet on port " + std::to_string(port) + ": " + ec.message(), "error");
                                }
                            });
                    }
                });
        }

        // Запускаем io_context в отдельном потоке
        std::thread ioThread([this]() {
            ioContext.run();
        });

        // Ожидаем завершение работы io_context
        ioThread.join();
    } catch (const std::exception& e) {
        logMessage("Error starting UDP Proxy: " + std::string(e.what()), "error");
    }
}

void UdpProxy::handleIncomingPacket(asio::ip::udp::socket& socket, std::size_t bytesReceived) {
    std::string clientIp = clientEndpoint.address().to_string();
    int clientPort = clientEndpoint.port();

    if (isIpBlocked(clientIp, ipMap, 10, 600)) {
        logMessage("IP blocked: " + clientIp, "error");
        return;
    }

    addIpConnection(clientIp, ipMap, 10, 600);

    logMessage("Received packet from " + clientIp + ":" + std::to_string(clientPort), "proxy");

    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::endpoint remoteEndpoint = *resolver.resolve(remoteServerIp, std::to_string(remoteServerPort)).begin();

        socket.async_send_to(
            asio::buffer(buffer, bytesReceived), remoteEndpoint,
            [clientIp](std::error_code ec, std::size_t bytesSent) {
                if (ec) {
                    logMessage("Error sending packet to remote server for IP: " + clientIp + ": " + ec.message(), "error");
                }
            });
    } catch (const std::exception& e) {
        logMessage("Error handling packet for IP: " + clientIp + ": " + std::string(e.what()), "error");
    }
}