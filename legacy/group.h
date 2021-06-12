#pragma once

#include "executer.h"

#include <vector>
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

    std::size_t nextId;

    std::vector<std::unique_ptr<Executer>> instances;
};
