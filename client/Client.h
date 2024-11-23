//
// Created by Андрей Степанов on 23.11.2024.
//
#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>

class Client {
    int client;
    sockaddr_in serverAddr{};

    char data[255]{};
    char buf[255]{};
public:
    Client();

    ~Client();

    void run();
private:
    bool connectToServer();
};


#endif //CLIENT_H
