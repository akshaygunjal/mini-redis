#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

class Store {
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> data;
    std::mutex mtx;
};