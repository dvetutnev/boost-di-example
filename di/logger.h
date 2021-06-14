#pragma once

#include <iostream>
#include <mutex>


struct ILogger
{
    virtual void log(const std::string&) = 0;
    virtual ~ILogger() = default;
};


class Logger : public ILogger
{
public:
    void log(const std::string& s) override {
        std::lock_guard lock{mtx};
        std::cout << s << std::endl;
    }

private:
    std::mutex mtx;
};
