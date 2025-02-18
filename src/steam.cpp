// src/steam.cpp
#include "steam.h"
#include <random>
#include <ctime>
#include <iostream>

Steam::Steam(asio::io_context& ioContext)
    : socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)) {
    initializeMasterConnection();
}

void Steam::initializeMasterConnection() {
    // Настройка подключения к Steam Master Server
    masterEndpoint = asio::ip::udp::endpoint(asio::address::from_string("127.0.0.1"), 27011);
    std::cout << "Подключение к Steam Master Server: " << masterEndpoint << std::endl;
}

void Steam::start() {
    // TODO: Реализовать логику обработки запросов от Steam Master Server
    std::cout << "Steam модуль запущен" << std::endl;
}

void Steam::sendChallengeResponse(uint32_t challenge) {
    ChallengePacket packet;
    packet.challenge = challenge;

    std::vector<uint8_t> buffer(sizeof(ChallengePacket));
    memcpy(buffer.data(), &packet, sizeof(ChallengePacket));

    socket.async_send_to(
        asio::buffer(buffer), masterEndpoint,
        [](std::error_code ec, [[maybe_unused]] std::size_t /*bytesSent*/) {
            if (ec) {
                std::cerr << "Ошибка отправки challenge-ключа: " << ec.message() << std::endl;
            } else {
                std::cout << "Challenge-ключ успешно отправлен" << std::endl;
            }
        });
}

void Steam::sendServerUpdate() {
    std::vector<uint8_t> buffer;
    // TODO: Заполнить buffer данными о состоянии сервера

    socket.async_send_to(
        asio::buffer(buffer), masterEndpoint,
        [](std::error_code ec, [[maybe_unused]] std::size_t /*bytesSent*/) {
            if (ec) {
                std::cerr << "Ошибка отправки обновления состояния: " << ec.message() << std::endl;
            }
        });
}
