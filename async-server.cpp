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

#define YESCRYPT_P 1

// for convenience
using json = nlohmann::json;
using namespace nlohmann::literals;

using boost::asio::ip::tcp;
using namespace std::chrono_literals;

class LogFile {
   static int x;
	std::mutex mutex_;
    std::ofstream ofsm_;
	std::once_flag flag_;
    void init() { ofsm_.open("log.txt"); }
public:
	void shared_print(std::string log_value) {
      std::call_once(flag_, &LogFile::init, this); // init() will only be called once by one thread
		//std::call_once(m_flag, [&](){ f.open("log.txt"); });  // Lambda solution
		//std::call_once(_flag, [&](){ _f.open("log.txt"); });  // file will be opened only once by one thread
		ofsm_ << log_value << std::endl;
	}
    void operator()(std::string log_value) {
      std::call_once(flag_, &LogFile::init, this); // init() will only be called once by one thread
		//std::call_once(flag, [&](){ f.open("log.txt"); });  // Lambda solution
		//std::call_once(flag, [&](){ _f.open("log.txt"); });  // file will be opened only once by one thread
		ofsm_ << log_value << std::endl;
	}
};

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

// A pass_key is a technique to restrict the construction of a class to only certain friends.
// It is often used to enforce that only certain functions or classes can create instances of a class.
// This is a common pattern in C++ to control access to constructors and prevent misuse of the class.
// It is a way to implement the "pimpl" idiom or to restrict access to certain constructors.
// The pass_key class is a simple utility that allows only the specified type T to construct instances of 
// the class that uses it.
template <typename T>class pass_key{  friend T;    explicit pass_key() = default;};
// The special class is an example of a class that uses the pass_key technique to restrict its constructor.
class special {
    public:    special(pass_key<special>, int x);    
    static std::unique_ptr<special> make(int x){        
            return std::make_unique<special>(pass_key<special>{}, x);    
        }
    };

class Secret {
  class ConstructorKey {
    friend class SecretFactory;
  private:
    ConstructorKey() {};
    ConstructorKey(ConstructorKey const&) = default;
  };
public:
  //Whoever can provide a key has access:
  explicit Secret(std::string str, ConstructorKey) : data(std::move(str)) {}
private:
  //these stay private, since Secret itself has no friends any more
  void addData(std::string const& moreData);
  std::string data;
};
class SecretFactory {
public:
  Secret getSecret(std::string str) {
    return Secret{std::move(str), {}}; //OK, SecretFactory can access
  }
  // void modify(Secret& secret, 
  // std::string const& additionalData) {
  //   secret.addData(additionalData);   //ERROR: void Secret::addData(const string&) is private
  // }
};

namespace ns
{
class user
{
  private:
    std::string email_;
    std::string username_;
    std::string salt_{"SodiumChloride"} ; // The salt is used to hash the password securely.
    // The salt is a random value that is used to hash the password securely.
     // The password should be stored securely, e.g., hashed and salted.
    // For simplicity, we are storing it as plain text here, which is not recommended in production code.
    // In a real application, you would use a secure hashing algorithm like bcrypt or Argon2.
    // and store the hash instead of the plain text password.
    // You would also want to implement proper validation and error handling.
    // Consider using a library like OpenSSL or libsodium for secure password hashing.
    // You can also use a library like Crypto++ or Botan for cryptographic operations.      
    std::string password_;
    // The pass_key is a technique to restrict the construction of a class to only certain friends.
    // It is often used to enforce that only certain functions or classes can create instances of a class.
    // It is a common pattern in C++ to control access to constructors and prevent misuse of the class.
    // It is a way to implement the "pimpl" idiom or to restrict access to certain constructors.
    // The pass_key class is a simple utility that allows only the specified type T to construct instances of the class that uses it.
    std::string public_pass_key_;
    std::string password_hash_;
    std::string hash_password(const std::string& password) {
            // Placeholder for actual hashing logic
            return password; // In a real application, replace this with a secure hash
        }
    
    // std::string hash_password(const std::string& password) {
    //     // Placeholder for actual hashing logic     
    //     return password; // In a real application, replace this with a secure hash
    // }
 
  public:
    user() = default;
    user(std::string email, std::string username, std::string password, std::string public_pass_key)
        : email_(std::move(email)), username_(std::move(username)), 
            password_(std::move(password)), public_pass_key_(std::move(public_pass_key)) // Placeholder for public key
    {}

    // std::string hash_password() const {
    //     return hash_password(password_);
    // }
    std::string get_hashed_password() const {
        return  "" ; //hash_password();
    }

    std::string get_password() const {
        return password_;
    }
    std::string get_email() const {
        return email_;
    }
    std::string get_username() const {
        return username_;
    }

    std::string get_public_pass_key() const {
        return public_pass_key_;
    }

    void set_hashed_password() const {
        
    }

    void set_password() const {
        
    }
    void set_email() const {
        // Placeholder for setting email logic
    }
    void set_username() const {
        // Placeholder for setting username logic
    }

    void set_public_pass_key() const {
        // Placeholder for setting public key logic
    }

    // Modern C++ yescrypt password hashing function
    // Returns the hash as a std::string, or empty string on error
    std::string yescrypt_password(const std::string& password, const std::string& salt) {
        yescrypt_local_t local;
        if (yescrypt_init_local(&local)) {
            std::cerr << "yescrypt_init_local() FAILED" << std::endl;
            return "";
        }
        yescrypt_params_t params = {
            YESCRYPT_DEFAULTS, // flags
            1 << 14,           // N (memory cost, 16384)
            8,                 // r (block size)
            1,                 // p (parallelism)
            0, 0, 0            // t, g, dkLen (not used)
        };
        // yescrypt hash output buffer
        char hash[128] = {0};
        // Use yescrypt_kdf for password hashing
        int result = yescrypt_kdf(
            nullptr, // shared context (not used)
            &local,  // local context
            reinterpret_cast<const uint8_t*>(password.data()), password.size(),
            reinterpret_cast<const uint8_t*>(salt.data()), salt.size(),
            &params,
            reinterpret_cast<uint8_t*>(hash), sizeof(hash)
        );
        yescrypt_free_local(&local);
        if (result) {
            std::cerr << "yescrypt_kdf() FAILED" << std::endl;
            return "";
        }
        // Return the hash as a hex string
        std::ostringstream oss;
        for (size_t i = 0; i < 64; ++i) // 64 bytes = 512 bits
            oss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned>(static_cast<uint8_t>(hash[i])));
        return oss.str();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(user, email_, username_, password_, public_pass_key_)
    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(user, email_, username_, password_, pass_key)
    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(user, email_, username_, password_)
    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(user, email_, username_, password_hash)
};
} // namespace ns

class UserManager {
public:
    static UserManager& instance() {
        static UserManager inst;
        return inst;
    }
    // bool register_user(const std::string& email, const std::string& username, const std::string& password) {
    bool register_user(ns::user user) {
        std::lock_guard<std::mutex> lock(mutex_);        
        json user_json = user; // Convert user to JSON
        std::string username = user_json["username_"].get<std::string>();        
        std::string password = user_json["password_"].get<std::string>();
        std::string email = user_json["email_"].get<std::string>();
        std::string public_pass_key = user_json["public_pass_key_"].get<std::string>();        
        //  std::cout << "serialization: " << user_json << std::endl;

        if (users_.count(username)) return false; // Already registered
        // users_[username] = user_json; // Store user with email, username, and password
        users_.emplace(username, user); // Store user object
        // users_[username] = password;
        for (auto& element : users_) {
            std::cout << "User: " << element.first << ", Email: " << element.second.get_email() 
                      << ", Password: " << element.second.get_password() 
                      << ", Public Key: " << element.second.get_public_pass_key() << std::endl;
        }
        return true;
    }
    bool is_registered(const std::string& username) {
        std::lock_guard<std::mutex> lock(mutex_);
        return users_.count(username) > 0;
    }
    bool authenticate(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(username);
        if (it == users_.end()) return false; // User not found
        json p2 = it->second; // Get the user JSON
        std::cout << "Deserialization: " << p2.dump() << std::endl;
        
        // Check if the password matches
        //return it != users_.end() && p2[password] == password    //it->second == password;
        return true; // For demo purposes, always return true
    }
private:
    std::mutex mutex_;
    std::map<std::string, ns::user> users_;
};

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
