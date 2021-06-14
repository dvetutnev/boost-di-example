#pragma once

#include "executer.h"

#include <list>
#include <memory>


struct IGroup
{
    virtual IExecuter& getExecuter() = 0;
    virtual void stopAll() = 0;
    virtual ~IGroup() = default;
};


class Group : public IGroup
{
public:
    Group(const Ssid&, std::size_t, ILogger&);

    IExecuter& getExecuter() override;
    void stopAll() override;

private:
    const Ssid ssid;
    const std::size_t maxSize;

    ILogger& logger;

    std::size_t currentId;

    using Container = std::list<std::unique_ptr<IExecuter>>;
    Container instances;
    Container::iterator it;
};
