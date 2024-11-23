#include "Server.h"

Server::Server() {
    std::cout << "This is server" << std::endl;
    createSocket();
    bindAddress();
}

Server::~Server() {
    close(listenfd);  // Закрываем сокет прослушивания
    for (auto &t : clientThreads) {
        if (t.joinable()) {
            t.join();  // Ждем завершения всех потоков
        }
    }
}

void Server::run() {
    listenToClient();
    while (true) {
        acceptConnection();  // Продолжаем принимать подключения

        // Создаем новый поток для каждого нового клиента
        std::lock_guard<std::mutex> lock(acceptMutex);  // Защищаем создание потока

        clientThreads.push_back(std::thread(&Server::handleClient, this));
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
    if(listen(listenfd, 5) == -1) {
        std::cerr << "Error: listen" << std::endl;
        throw std::runtime_error("listen");
    }
    std::cout << "...listening" << std::endl;
}

void Server::acceptConnection() {
    conn = accept(listenfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
    if (conn < 0) {
        std::cerr << "Error: accept" << std::endl;
        return;
    }
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
}

void Server::handleClient() {
    char buf[255];
    while (true) {
        memset(buf, 0, sizeof(buf));
        const long len = recv(conn, buf, sizeof(buf), 0);
        if (len <= 0) {
            std::cerr << "Client disconnected" << std::endl;
            break;
        }
        buf[len] = '\0';
        if (strcmp(buf, "exit") == 0) {
            std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
            break;
        }
        std::cout << buf << std::endl;
        buf[1] = 'o';  // Пример обработки данных
        send(conn, buf, len, 0);
    }
    close(conn);  // Закрываем соединение с клиентом после завершения
}
