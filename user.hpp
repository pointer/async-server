#pragma once
#include <string>
#include <array>
#include <nlohmann/json.hpp>

namespace ns {
class user {
private:
    std::string email_;
    std::string username_;
    std::string salt_{"SodiumChloride"};
    std::string password_;
    std::string public_pass_key_;
    std::string password_hash_;
    std::string hash_password(const std::string& password);
public:
    user() = default;
    user(std::string email, std::string username, std::string password, std::string public_pass_key);
    std::string get_hashed_password() const;
    std::string get_password() const;
    std::string get_email() const;
    std::string get_username() const;
    std::string get_public_pass_key() const;
    void set_hashed_password() const;
    void set_password() const;
    void set_email() const;
    void set_username() const;
    void set_public_pass_key() const;
    std::string yescrypt_password(const std::string& password, const std::string& salt);
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(user, email_, username_, password_, public_pass_key_)
};
} // namespace ns
