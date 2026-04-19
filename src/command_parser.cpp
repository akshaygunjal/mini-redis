#include "command_parser.hpp"
#include "store.hpp"
#include <sstream>

static Store store;

std::string CommandParser::execute(const std::string& command) {

    std::stringstream ss(command);

    std::string cmd, key, value;
    ss >> cmd >> key;
    std::getline(ss, value);

    // remove leading space
    if (!value.empty() && value[0] == ' ')
        value.erase(0, 1);

    if (cmd == "PING")
        return "PONG\n";

    if (cmd == "SET") {
        
        int ttl = -1;

        size_t pos = value.find(" EX ");

        if(pos != std::string::npos){

            std::string ttl_str = value.substr(pos + 4);
            ttl = std::stoi(ttl_str);
            value = value.substr(0,pos);
        }
        
        store.set(key, value, ttl);
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