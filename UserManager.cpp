#include "UserManager.hpp"
#include <iostream>

UserManager& UserManager::instance() {
    static UserManager inst;
    return inst;
}

bool UserManager::register_user(ns::user user) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json user_json = user;
    std::string username = user_json["username_"].get<std::string>();
    if (users_.count(username)) return false;
    users_.emplace(username, user);
    for (auto& element : users_) {
        std::cout << "User: " << element.first << ", Email: " << element.second.get_email()
                  << ", Password: " << element.second.get_password()
                  << ", Public Key: " << element.second.get_public_pass_key() << std::endl;
    }
    return true;
}

bool UserManager::is_registered(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    return users_.count(username) > 0;
}

bool UserManager::authenticate(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(username);
    if (it == users_.end()) return false;
    nlohmann::json p2 = it->second;
    std::cout << "Deserialization: " << p2.dump() << std::endl;
    // For demo: always return true
    return true;
}
