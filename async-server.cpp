#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>



using boost::asio::ip::tcp;

struct SessionInfo {
    std::string username;
    uint64_t session_id;
    std::chrono::system_clock::time_point start_time;
};

class SessionManager {
public:
    static SessionManager& instance() {
        static SessionManager inst;
        return inst;
    }
    uint64_t register_session(const std::string& username) {
        std::lock_guard<std::mutex> lock(mutex_);
        uint64_t id = ++last_id_;
        sessions_[id] = SessionInfo{username, id, std::chrono::system_clock::now()};
        std::cout << "Session registered: " << username << ", id=" << id << std::endl;
        return id;
    }
    void remove_session(uint64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(id);
        if (it != sessions_.end()) {
            std::cout << "Session removed: " << it->second.username << ", id=" << id << std::endl;
            sessions_.erase(it);
        }
    }
    void print_sessions() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "Active sessions: " << sessions_.size() << std::endl;
        for (const auto& [id, info] : sessions_) {
            std::cout << "  id=" << id << ", user=" << info.username << std::endl;
        }
    }
private:
    std::mutex mutex_;
    std::map<uint64_t, SessionInfo> sessions_;
    std::atomic<uint64_t> last_id_{0};
};

boost::asio::awaitable<void> echo(tcp::socket socket) {
    char data[1024];
    // Simple authentication: hardcoded username and password
    const std::string valid_username = "user";
    const std::string valid_password = "pass";
    std::string prompt = "Username: ";
    co_await boost::asio::async_write(socket, boost::asio::buffer(prompt), boost::asio::use_awaitable);
    std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
    std::string username(data, bytes_read);
    username.erase(username.find_last_not_of("\r\n") + 1);
    prompt = "Password: ";
    co_await boost::asio::async_write(socket, boost::asio::buffer(prompt), boost::asio::use_awaitable);
    bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
    std::string password(data, bytes_read);
    password.erase(password.find_last_not_of("\r\n") + 1);
    if (username != valid_username || password != valid_password) {
        std::string fail = "Authentication failed. Closing connection.\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
        co_return;
    } else {
        std::string success = "Authentication successful. Welcome!\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(success), boost::asio::use_awaitable);
    }
    // Register session
    uint64_t session_id = SessionManager::instance().register_session(username);

    while (true) {
        std::cout << "Reading data from socket...\n";
        std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);

        if (bytes_read == 0) {
            std::cout << "No data. Exiting loop...\n";
            break;
        }

        // Remove /r/n from end of received data
        std::string str(data, bytes_read);
        if (!str.empty() && str.back() == '\n') {
            str.pop_back();
        }
        if (!str.empty() && str.back() == '\r') {
            str.pop_back();
        }

        // If the client send QUIT, the server closes the connection
        if (str == "QUIT") {
            std::string bye("Good bye!\n");
            co_await boost::asio::async_write(socket, boost::asio::buffer(bye), boost::asio::use_awaitable);
            break;
        }
        // Special command: LIST_SESSIONS (for demo)
        if (str == "LIST_SESSIONS") {
            std::ostringstream oss;
            oss << "Active sessions:\n";
            SessionManager::instance().print_sessions();
            std::string msg = "Session list printed to server console.\n";
            co_await boost::asio::async_write(socket, boost::asio::buffer(msg), boost::asio::use_awaitable);
            continue;
        }
        std::cout << "Writing '" << str << "' back into the socket...\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(data, bytes_read), boost::asio::use_awaitable);
    }
    // Remove session on disconnect
    SessionManager::instance().remove_session(session_id);
}

boost::asio::awaitable<void> listener(boost::asio::io_context& io_context, unsigned short port) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

    while (true) {
        std::cout << "Accepting connections...\n";
        tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);

        std::cout << "Starting an Echo connection handler...\n";
        boost::asio::co_spawn(io_context, echo(std::move(socket)), boost::asio::detached);
    }
}

int main() {
    boost::asio::io_context io_context;

    try {
        boost::asio::co_spawn(io_context, listener(io_context, 12345), boost::asio::detached);
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        io_context.stop();
    }

    return 0;
}
