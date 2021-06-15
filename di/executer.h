#pragma once

#include "logger.h"

#include <boost/asio/io_context.hpp>

#include <functional>
#include <thread>
#include <memory>


struct Ssid
{
    std::string value;
};

struct Id
{
    std::string value;
};


struct IExecuter
{
    virtual void process(const std::string&, std::function<void(const std::string&)>) = 0;
    virtual void stop() = 0;
    virtual const Id& getId() const = 0;
    virtual ~IExecuter() = default;
};


class Executer : public IExecuter
{
public:
    Executer(const Ssid&, const Id&, std::shared_ptr<ILogger>);

    void process(const std::string&, std::function<void(const std::string&)>) override;
    void stop() override;

    const Id& getId() const override;

private:
    const Ssid ssid;
    const Id id;

    const std::shared_ptr<ILogger> logger;

    boost::asio::io_context ioContext;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    WorkGuard workGuard;

    std::thread thread;
    void worker();
};


inline bool operator== (const Ssid& a, const Ssid& b) {
    return a.value == b.value;
}

inline bool operator== (const Id& a, const Id& b) {
    return a.value == b.value;
}
