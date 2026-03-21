#include "client_session.hpp"
#include "command_parser.hpp"

#include <unistd.h>
#include <iostream>

void ClientSession::handle(int client_fd) {

    char buffer[1024];
    std::string connection_buffer;

    while (true) {

        int bytes_read = read(client_fd, buffer, sizeof(buffer));

        if (bytes_read <= 0)
            break;

        connection_buffer.append(buffer, bytes_read);

        while (true) {

            size_t pos = connection_buffer.find('\n');

            if (pos == std::string::npos)
                break;

            std::string command = connection_buffer.substr(0, pos);
            connection_buffer.erase(0, pos + 1);

            std::cout << "Command received: " << command << "\n";

            std::string response = CommandParser::execute(command);

            write(client_fd, response.c_str(), response.size());
        }
    }

    close(client_fd);
}