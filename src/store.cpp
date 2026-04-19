#include "store.hpp"
#include <iostream>

void Store::set(const std::string& key, const std::string& value,int ttl) {
    std::lock_guard<std::mutex> lock(mtx);
    time_t expiry = (ttl == -1) ? -1 : time(nullptr) + ttl;
    data[key] = {value, expiry};
    std::cout << "SET -> key: " << key 
          << " expiry: " << expiry << std::endl;
}

std::string Store::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!data.count(key))
        return "NULL";

    auto it = data.find(key);

    if (it == data.end())
        return "NULL";

    auto& value = it->second.first;
    auto& expiry = it->second.second;

    if(expiry != -1 && time(nullptr) > expiry){
        data.erase(key);
        return "NULL";
    }


    std::cout << "GET -> current: " << time(nullptr)
          << " expiry: " << expiry << std::endl;
    return value;
}

void Store::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    data.erase(key);
}