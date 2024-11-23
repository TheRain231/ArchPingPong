// Created by Андрей Степанов on 23.11.2024.

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Server {
    int listenfd{};                          // Сокет для прослушивания
    sockaddr_in addr{};                      // Адрес сервера

    std::vector<std::thread> clientThreads;  // Вектор потоков клиентов
    std::map<int, std::string> clients;      // Карта клиентов (сокет -> IP)
    std::mutex clientsMutex;                 // Мьютекс для защиты clients

public:
    Server();
    ~Server();

    [[noreturn]] void run();

private:
    void createSocket();
    void bindAddress() const;
    void listenToClient() const;
    void acceptConnection();
    void handleClient(int clientSocket);
};

#endif //SERVER_H
