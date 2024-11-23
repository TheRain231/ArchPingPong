//
// Created by Андрей Степанов on 23.11.2024.
//

#include "Client.h"

#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(): client(socket(AF_INET, SOCK_STREAM, 0)) {
    std::cout << "This is client" << std::endl;

    connectToServer();

    std::cout << "...connected" << std::endl;
}

Client::~Client() {
    close(client);
}

void Client::run() {
    while (true) {
        std::cin >> data;
        if (send(client, data, strlen(data), 0) < 0) {
            std::cerr << "Failed to send data. Server might be disconnected." << std::endl;
            break;
        }

        memset(buf, 0, sizeof(buf));
        long len = recv(client, buf, sizeof(buf), 0);

        if (len == 0) {
            std::cerr << "Server disconnected." << std::endl;

            connectToServer();
        } else if (len < 0) {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break; // Ошибка получения данных
        }

        buf[len] = '\0';
        std::cout << buf << std::endl;
    }
}

bool Client::connectToServer() {
    while (true) {
        client = socket(AF_INET, SOCK_STREAM, 0);
        if (client < 0) {
            std::cerr << "Error: failed to create socket." << std::endl;
            return false;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8000);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(client, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == 0) {
            std::cout << "Connected to server!" << std::endl;
            return true;
        }
        std::cerr << "Connection failed: " << strerror(errno) << ". Retrying in " << 1 << " second..." << std::endl;
        sleep(1);
    }
}