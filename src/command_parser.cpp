#include "command_parser.hpp"
#include "store.hpp"
#include <sstream>

static Store store;

std::string CommandParser::execute(const std::string& command) {

    std::stringstream ss(command);

    std::string cmd, key, value;
    ss >> cmd >> key >> value;

    if (cmd == "PING")
        return "PONG\n";

    if (cmd == "SET") {
        store.set(key, value);
        return "OK\n";
    }

    if (cmd == "GET") {
        return store.get(key) + "\n";
    }

    if (cmd == "DEL") {
        store.del(key);
        return "OK\n";
    }

    return "ERR unknown command\n";
}