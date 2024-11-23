// Created by Андрей Степанов on 23.11.2024.

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>   // Для мьютекса

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Server {
    int listenfd{};                          // Сокет для прослушивания
    sockaddr_in addr{};                      // Адрес сервера

    int conn{};                              // Сокет для подключения клиента
    char clientIP[INET_ADDRSTRLEN] = "";     // IP клиента

    sockaddr_in clientAddr{};                // Адрес клиента
    socklen_t clientAddrLen = sizeof(clientAddr); // Длина адреса клиента

    std::vector<std::thread> clientThreads;  // Вектор для хранения потоков клиентов
    std::mutex acceptMutex;                  // Мьютекс для синхронизации

public:
    Server();
    ~Server();

    [[noreturn]] void run();

private:
    void createSocket();                     // Создание сокета
    void bindAddress() const;                // Привязка адреса
    void listenToClient() const;             // Прослушивание клиента
    void acceptConnection();                 // Принятие подключения
    void handleClient();                     // Обработка клиента в отдельном потоке
};

#endif //SERVER_H
