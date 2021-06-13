#pragma once

#include "group.h"
#include <map>


class Manager
{
public:
    Manager(std::size_t, Logger&);

    Executer& getExecuter(const Ssid&);
    void stop(const Ssid&);
    void stopAll();

private:
    const std::size_t groupSize;
    Logger& logger;

    using Item = std::unique_ptr<Group>;
    struct Compare
    {
        bool operator()(const Ssid& a, const Ssid& b) const {
            return std::less<std::string>{}(a.value, b.value);
        }
    };
    std::map<Ssid, Item, Compare> groups;
};
