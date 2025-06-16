#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <future>

class LogFile {
    static int x;
    std::mutex mutex_;
    std::ofstream ofsm_;
    std::once_flag flag_;
    void init();
public:
    void shared_print(std::string log_value);
    void operator()(std::string log_value);
};
