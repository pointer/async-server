#pragma once
#include <string>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include "user.hpp"

class UserManager {
public:
    static UserManager& instance();
    bool register_user(ns::user user);
    bool is_registered(const std::string& username);
    bool authenticate(const std::string& username, const std::string& password);
private:
    std::mutex mutex_;
    std::map<std::string, ns::user> users_;
};
