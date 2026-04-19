#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <ctime>

class Store {
public:
    void set(const std::string& key, const std::string& value,int ttl = -1);
    std::string get(const std::string& key);
    void del(const std::string& key);

private:
    std::unordered_map<std::string, std::pair<std::string,time_t>> data;
    std::mutex mtx;
};