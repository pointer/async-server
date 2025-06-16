#include "LogFile.hpp"

void LogFile::init() {
    ofsm_.open("log.txt");
}

void LogFile::shared_print(std::string log_value) {
    std::call_once(flag_, &LogFile::init, this);
    ofsm_ << log_value << std::endl;
}

void LogFile::operator()(std::string log_value) {
    std::call_once(flag_, &LogFile::init, this);
    ofsm_ << log_value << std::endl;
}
