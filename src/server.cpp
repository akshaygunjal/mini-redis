#include "server.hpp"
#include "client_session.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>

void Server::start(int port) {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    std::cout << "Server listening on port " << port << "\n";

    while (true) {

        int client_fd = accept(server_fd, nullptr, nullptr);

        std::cout << "Client connected\n";

        std::thread client_thread(ClientSession::handle, client_fd);

        client_thread.detach();
    }
}