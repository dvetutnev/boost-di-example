#pragma once

#include "executer.h"

#include <boost/di/extension/injections/factory.hpp>
#include <list>


struct IGroup
{
    virtual IExecuter& getExecuter() = 0;
    virtual void stopAll() = 0;
    virtual ~IGroup() = default;
};


struct GroupSize
{
    std::size_t value;
};


using IFactoryExecuter = boost::di::extension::ifactory<IExecuter, Ssid, Id>;


class Group : public IGroup
{
public:
    Group(const Ssid&, GroupSize, std::shared_ptr<IFactoryExecuter>);

    IExecuter& getExecuter() override;
    void stopAll() override;

private:
    const Ssid ssid;
    const std::size_t maxSize;

    const std::shared_ptr<IFactoryExecuter> factory;

    std::size_t currentId;

    using Container = std::list<std::unique_ptr<IExecuter>>;
    Container instances;
    Container::iterator it;
};
