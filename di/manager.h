#pragma once

#include "group.h"

#include <boost/di/extension/injections/factory.hpp>
#include <map>


using IFactoryGroup = boost::di::extension::ifactory<IGroup, const Ssid&>;


class Manager
{
public:
    Manager(std::shared_ptr<IFactoryGroup>);

    IExecuter& getExecuter(const Ssid&);
    void stop(const Ssid&);
    void stopAll();

private:
    const std::shared_ptr<IFactoryGroup> factory;

    struct Compare
    {
        bool operator()(const Ssid& a, const Ssid& b) const {
            return std::less<std::string>{}(a.value, b.value);
        }
    };

    std::map<Ssid, std::unique_ptr<IGroup>, Compare> groups;
};
