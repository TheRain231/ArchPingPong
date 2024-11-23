#include "Server.h"

Server::Server() {
    std::cout << "This is server" << std::endl;
    createSocket();
    bindAddress();
}

Server::~Server() {
    close(listenfd);
    for (auto &t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Server::run() {
    listenToClient();
    while (true) {
        acceptConnection();
    }
}

void Server::createSocket() {
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        std::cerr << "Error: socket" << std::endl;
        throw std::runtime_error("Error: socket");
    }
}

void Server::bindAddress() const {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenfd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Error: bind" << std::endl;
        throw std::runtime_error("bind error");
    }
}

void Server::listenToClient() const {
    if (listen(listenfd, 5) == -1) {
        std::cerr << "Error: listen" << std::endl;
        throw std::runtime_error("listen");
    }
    std::cout << "...listening" << std::endl;
}

void Server::acceptConnection() {
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(listenfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << "Error: accept" << std::endl;
        return;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << " as Client " << clientSocket <<std::endl;
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients[clientSocket] = clientIP;  // Добавляем клиента в список
    }

    clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
}

void Server::handleClient(int clientSocket) {
    char buf[255];
    while (true) {
        memset(buf, 0, sizeof(buf));
        const long len = recv(clientSocket, buf, sizeof(buf), 0);
        if (len <= 0) {
            std::cerr << "Client disconnected: " << clientSocket << std::endl;
            break;
        }
        buf[len] = '\0';

        std::cout << "Client " << clientSocket << ": " << buf << std::endl;

        buf[1] = 'o';

        send(clientSocket, buf, len, 0);
    }

    close(clientSocket);
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(clientSocket);  // Удаляем клиента из списка
}