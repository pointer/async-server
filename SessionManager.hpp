#pragma once
#include <string>
#include <chrono>
#include <map>
#include <mutex>
#include <atomic>

struct SessionInfo {
    std::string username;
    uint64_t session_id;
    std::chrono::system_clock::time_point start_time;
};

class SessionManager {
public:
    static SessionManager& instance();
    uint64_t register_session(const std::string& username);
    void remove_session(uint64_t id);
    void print_sessions();
private:
    std::mutex mutex_;
    std::map<uint64_t, SessionInfo> sessions_;
    std::atomic<uint64_t> last_id_{0};
};
