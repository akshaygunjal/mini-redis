#include "store.hpp"

void Store::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    data[key] = value;
}

std::string Store::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    if (data.count(key))
        return data[key];
    return "NULL";
}

void Store::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    data.erase(key);
}