#pragma once

#include "executer.h"
#include <vector>


class Group
{
public:
    Group(Ssid, std::size_t);

    Executer& getExectuter();
    void stop();

private:
    const Ssid ssid;
    const std::size_t maxSize;

    std::vector<Executer> instances;
};
