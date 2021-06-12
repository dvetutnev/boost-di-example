#pragma once

#include "executer.h"

#include <list>
#include <memory>


class Group
{
public:
    Group(Ssid, std::size_t);

    Executer& getExecuter();
    void stopAll();

private:
    const Ssid ssid;
    const std::size_t maxSize;

    std::size_t currentId;

    using Container = std::list<std::unique_ptr<Executer>>;
    Container instances;
    Container::iterator it;
};
