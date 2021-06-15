#pragma once

#include "group.h"
#include <gmock/gmock.h>


struct MockLogger : ILogger
{
    MOCK_METHOD(void, log, (const std::string&), (override));
};

struct MockExecuter : IExecuter
{
    MOCK_METHOD(void, process, (const std::string&, std::function<void(const std::string&)>), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(const Id&, getId, (), (const, override));
};

struct MockFactoryExecuter : IFactoryExecuter
{
    MOCK_METHOD((std::unique_ptr<IExecuter>), create, (const Ssid&, const Id&), (const, override));
};
