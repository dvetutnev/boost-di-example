#pragma once

#include "logger.h"

#include <boost/asio/io_context.hpp>

#include <string>
#include <functional>
#include <thread>


struct Ssid
{
    std::string value;
};

struct Id
{
    std::string value;
};


class Executer
{
public:
    Executer(const Ssid&, const Id&, Logger&);

    void process(const std::string&, std::function<void(const std::string&)>);
    void stop();

private:
    const Ssid ssid;
    const Id id;

    Logger& logger;

    boost::asio::io_context ioContext;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    WorkGuard workGuard;

    std::thread thread;
    void worker();
};
