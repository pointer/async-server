#include "SessionManager.hpp"
#include <iostream>

SessionManager& SessionManager::instance() {
    static SessionManager inst;
    return inst;
}

uint64_t SessionManager::register_session(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint64_t id = ++last_id_;
    sessions_[id] = SessionInfo{username, id, std::chrono::system_clock::now()};
    std::cout << "Session registered: " << username << ", id=" << id << std::endl;
    return id;
}

void SessionManager::remove_session(uint64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(id);
    if (it != sessions_.end()) {
        std::cout << "Session removed: " << it->second.username << ", id=" << id << std::endl;
        sessions_.erase(it);
    }
}

void SessionManager::print_sessions() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Active sessions: " << sessions_.size() << std::endl;
    for (const auto& [id, info] : sessions_) {
        std::cout << "  id=" << id << ", user=" << info.username << std::endl;
    }
}
