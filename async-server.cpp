#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <thread>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
extern "C" {
#include "yescrypt.h"
}

#include "LogFile.hpp"
#include "SessionManager.hpp"
#include "user.hpp"
#include "UserManager.hpp"

#define YESCRYPT_P 1

// for convenience
using json = nlohmann::json;
using namespace nlohmann::literals;

using boost::asio::ip::tcp;
using namespace std::chrono_literals;

boost::asio::awaitable<void> handler(tcp::socket socket) {
    char data[1024];
    std::timed_mutex mutex;
    std::mutex mtx;

    std::unique_lock<std::timed_mutex> lock(mutex, std::try_to_lock);
    if(!lock.owns_lock()) {
        std::cout << "Failed to acquire lock, closing connection.\n";
        co_return;
    }
    std::cout << "Connection established. Waiting for authentication or registration...\n";

    std::string email, username, password, public_key;
    std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
    if (bytes_read == 0) {
        std::cout << "No data received. Closing connection.\n";
        co_return;
    }

    std::string reg_info(data, bytes_read);
    reg_info.erase(reg_info.find_last_not_of("\r\n") + 1);
    // std::size_t pos = reg_info.find('@');
    std::string::size_type pos = 0;
    if (reg_info.find('@') != std::string::npos) {
        // std::string reg_info = reg_info.substr(9); // after "REGISTER "
        pos = reg_info.find(',');
        if (pos == std::string::npos) {
            std::string fail = "Invalid registration format. Use 'REGISTER, email, username,password'.\n";
            co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
            co_return;
        }
        email = reg_info.substr(0, pos);
        std::string username_pass = reg_info.substr(pos + 1);
        pos = username_pass.find(',');
        username = username_pass.substr(0, pos);
        password = username_pass.substr(pos + 1);        
        public_key = username_pass.substr(pos + 1);          
        ns::user p(email, username, password, public_key); // Placeholder for public key
        if (UserManager::instance().register_user(p)) {
            std::string success = "Registration successful. You can now login.\n";
            co_await boost::asio::async_write(socket, boost::asio::buffer(success), boost::asio::use_awaitable);
        } else {
            std::string fail = "Username already registered.\n";
            co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
        }
        // co_return;
    }

    // std::string email, username, password;
    bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
    if (bytes_read == 0) {
        std::cout << "No data received. Closing connection.\n";
        co_return;
    }
    std::string login_info(data, bytes_read);
    // Otherwise, treat as login: "username,password"
    pos = login_info.find(',');
    if (pos == std::string::npos) {
        std::string fail = "Invalid input format. Use 'username,password' or 'REGISTER username,password'.\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
        co_return;
    }
    username = login_info.substr(0, pos);
    password = login_info.substr(pos + 1);

    if (!UserManager::instance().is_registered(username)) {
        std::string fail = "User not registered. Please register first.\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
        co_return;
    }
    if (!UserManager::instance().authenticate(username, password)) {
        std::string fail = "Authentication failed. Closing connection.\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(fail), boost::asio::use_awaitable);
        co_return;
    } else {
        std::string success = "Authentication successful. Welcome!\n";
        co_await boost::asio::async_write(socket, boost::asio::buffer(success), boost::asio::use_awaitable);
    }

    // lock.unlock(); // Unlock the mutex after authentication
    // Register session    
    uint64_t session_id = SessionManager::instance().register_session(username);

    std::stringstream ss_log_message("");
    ss_log_message << "Session started: " << username << ", id=" << session_id 
           << ", time=" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n";
    static LogFile log;
    // std::string log_message = ss_log_message.str();
    std::future<void> futre = std::async(&LogFile::shared_print, &log, ss_log_message.str());
    lock.unlock(); // Unlock the mutex after authentication    
    while (true) {
        std::cout << "Reading data from socket...\n";
        std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), 
                        boost::asio::use_awaitable);

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
        std::cout << "Reading str ..." << str <<"\n";
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
        
        ss_log_message.str(""); // Clear the stringstream
        ss_log_message.clear(); // Clear any error flags
        // Log the data received
        ss_log_message << "User: " << username << ", id= " << session_id << ", " << "Data: " << str
            << ", time=" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n";
        // std::string log_message = ss_log_message.str();
        std::future<void> futre = std::async(&LogFile::shared_print, &log, ss_log_message.str());
        std::cout << "Data written back to socket.\n";
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
        boost::asio::co_spawn(io_context, handler(std::move(socket)), boost::asio::detached);
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
