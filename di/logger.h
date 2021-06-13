#pragma once

#include <iostream>
#include <mutex>


class Logger
{
public:
    void log(const std::string& s) {
        std::lock_guard lock{mtx};
        std::cout << s << std::endl;
    }

private:
    std::mutex mtx;
};
