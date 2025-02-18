#include "server_emulator.h"
#include "utils.h"

ServerEmulator::ServerEmulator(const std::string& ipAddrParam, int portNumParam, asio::io_context& ioContextParam, const Config& configParam)
    : socket(ioContextParam, asio::ip::udp::endpoint(asio::ip::make_address(ipAddrParam), portNumParam)),
      ioContext(ioContextParam),
      config(configParam),
      ip(ipAddrParam),
      port(portNumParam),
      bound(false) {}

ServerEmulator::~ServerEmulator() {
    if (bound) {
        logMessage("Сервер на порту " + std::to_string(port) + " остановлен", "server");
    }
}

void ServerEmulator::listenForConnections() {
    if (!bound) {
        logMessage("Сервер не привязан к порту " + std::to_string(port), "error");
        return;
    }

    uint8_t buffer[1400];
    auto threadFunc = [this, buffer]() mutable {
        while (true) {
            asio::ip::udp::endpoint senderEndpoint;
            socket.async_receive_from(asio::buffer(buffer, sizeof(buffer)), senderEndpoint,
                                      [this, buffer, senderEndpoint](std::error_code ec, std::size_t bytesReceived) mutable {
                                          if (!ec && bytesReceived > 0) {
                                              if (buffer[0] == '\xFF' && buffer[1] == '\xFF' && buffer[2] == '\xFF' && buffer[3] == '\xFF') {
                                                  if (buffer[4] == 'i') {
                                                      handleA2SInfoRequest(senderEndpoint);
                                                  } else if (buffer[4] == 'p') {
                                                      handleA2SPlayerRequest(senderEndpoint);
                                                  }
                                              }
                                          } else {
                                              logMessage("Ошибка приёма пакета: " + ec.message(), "error");
                                          }
                                      });

            sendMasterServerInfo();
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    };

    std::thread(threadFunc).detach();
}

bool ServerEmulator::isBound() const {
    return bound;
}

void ServerEmulator::sendMasterServerInfo() {
    try {
        asio::ip::udp::resolver resolver(ioContext);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), config.masterServerIp, std::to_string(config.masterServerPort));

        resolver.async_resolve(query, [this](asio::error_code ec, asio::ip::udp::resolver::iterator it) {
            if (!ec && it != asio::ip::udp::resolver::iterator()) {
                asio::ip::udp::endpoint masterEndpoint = *it;

                uint8_t buffer[1400];
                size_t packetSize = createMasterServerPacket(buffer, sizeof(buffer), ip, port, config.serverName, config.mapName, config.players, config.maxPlayers, config.protocolVersion);

                socket.async_send_to(asio::buffer(buffer, packetSize), masterEndpoint,
                                    [masterIp = config.masterServerIp](asio::error_code ec) {
                                        if (ec) {
                                            logMessage("Ошибка отправки информации мастер-серверу " + masterIp + ": " + ec.message(), "error");
                                        } else {
                                            logMessage("Информация успешно отправлена мастер-серверу " + masterIp, "server");
                                        }
                                    });
            } else {
                logMessage("Ошибка разрешения адреса мастер-сервера: " + ec.message(), "error");
            }
        });
    } catch (const std::exception& e) {
        logMessage("Ошибка при отправке информации мастер-серверу: " + std::string(e.what()), "error");
    }
}

void ServerEmulator::handleA2SInfoRequest(const asio::ip::udp::endpoint& senderEndpoint) {
    try {
        uint8_t buffer[1400];
        size_t packetSize = createA2SInfoPacket(buffer, sizeof(buffer), config.serverName, config.mapName, config.players, config.maxPlayers, config.protocolVersion);

        socket.async_send_to(asio::buffer(buffer, packetSize), senderEndpoint,
                            [](asio::error_code ec) {
                                if (ec) {
                                    logMessage("Ошибка отправки ответа A2S_INFO: " + ec.message(), "error");
                                }
                            });
    } catch (const std::exception& e) {
        logMessage("Ошибка обработки запроса A2S_INFO: " + std::string(e.what()), "error");
    }
}

void ServerEmulator::handleA2SPlayerRequest(const asio::ip::udp::endpoint& senderEndpoint) {
    try {
        uint8_t buffer[1400];
        size_t packetSize = createA2SPlayerPacket(buffer, sizeof(buffer), config.playersList);

        socket.async_send_to(asio::buffer(buffer, packetSize), senderEndpoint,
                            [](asio::error_code ec) {
                                if (ec) {
                                    logMessage("Ошибка отправки ответа A2S_PLAYER: " + ec.message(), "error");
                                }
                            });
    } catch (const std::exception& e) {
        logMessage("Ошибка обработки запроса A2S_PLAYER: " + std::string(e.what()), "error");
    }
}
