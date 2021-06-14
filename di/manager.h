#pragma once

#include "group.h"
#include <map>


class Manager
{
public:
    Manager(std::size_t, std::shared_ptr<ILogger>);

    IExecuter& getExecuter(const Ssid&);
    void stop(const Ssid&);
    void stopAll();

private:
    const std::size_t groupSize;
    const std::shared_ptr<ILogger> logger;

    struct Compare
    {
        bool operator()(const Ssid& a, const Ssid& b) const {
            return std::less<std::string>{}(a.value, b.value);
        }
    };
    std::map<Ssid, std::unique_ptr<IGroup>, Compare> groups;
};
