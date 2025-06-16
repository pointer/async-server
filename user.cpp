#include "user.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>
extern "C" {
#include "yescrypt.h"
}

namespace ns {
user::user(std::string email, std::string username, std::string password, std::string public_pass_key)
    : email_(std::move(email)), username_(std::move(username)),
      password_(std::move(password)), public_pass_key_(std::move(public_pass_key)) {}

std::string user::hash_password(const std::string& password) {
    // Placeholder for actual hashing logic
    return password;
}

std::string user::get_hashed_password() const {
    return "";
}

std::string user::get_password() const {
    return password_;
}
std::string user::get_email() const {
    return email_;
}
std::string user::get_username() const {
    return username_;
}
std::string user::get_public_pass_key() const {
    return public_pass_key_;
}
void user::set_hashed_password() const {}
void user::set_password() const {}
void user::set_email() const {}
void user::set_username() const {}
void user::set_public_pass_key() const {}

std::string user::yescrypt_password(const std::string& password, const std::string& salt) {
    yescrypt_local_t local;
    if (yescrypt_init_local(&local)) {
        std::cerr << "yescrypt_init_local() FAILED" << std::endl;
        return "";
    }
    yescrypt_params_t params{
        YESCRYPT_DEFAULTS,
        1 << 14,
        8,
        1,
        0, 0, 0
    };
    std::array<uint8_t, 64> hash{};
    int result = yescrypt_kdf(
        nullptr,
        &local,
        reinterpret_cast<const uint8_t*>(password.data()), password.size(),
        reinterpret_cast<const uint8_t*>(salt.data()), salt.size(),
        &params,
        hash.data(), hash.size()
    );
    yescrypt_free_local(&local);
    if (result) {
        std::cerr << "yescrypt_kdf() FAILED" << std::endl;
        return "";
    }
    std::ostringstream oss;
    for (auto byte : hash)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(byte);
    return oss.str();
}
} // namespace ns
