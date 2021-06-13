#include "manager.h"
#include "async_result.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


namespace {

Logger logger;

} // Anonymous namespace


TEST(Manager, singleGroup) {
    Manager manager{2, logger};

    const Ssid ssid{"Eridanus"};

    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncResult::create(ioContext);
    auto handler1 = [p = promise1](const std::string& result) mutable {
        p(result);
    };
    manager.getExecuter(ssid).process("aSdFgH", handler1);

    auto [promise2, future2] = AsyncResult::create(ioContext);
    auto handler2 = [p = promise2](const std::string& result) mutable {
        p(result);
    };
    manager.getExecuter(ssid).process("AsDfGh", handler2);

    ioContext.run();
    manager.stop(ssid);

    std::string result1 = future1.get();
    std::string result2 = future2.get();

    EXPECT_TRUE(boost::algorithm::starts_with(result1, "AsDfGh"));
    EXPECT_TRUE(boost::algorithm::ends_with(result1, "[Eridanus-0]"));

    EXPECT_TRUE(boost::algorithm::starts_with(result2, "aSdFgH"));
    EXPECT_TRUE(boost::algorithm::ends_with(result2, "[Eridanus-1]"));
}

TEST(Manager, twoGroup) {
    Manager manager{1, logger};

    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncResult::create(ioContext);
    auto handler1 = [p = promise1](const std::string& result) mutable {
        p(result);
    };
    const Ssid ssid1{"Cygnus"};
    manager.getExecuter(ssid1).process("aSdFgH", handler1);

    auto [promise2, future2] = AsyncResult::create(ioContext);
    auto handler2 = [p = promise2](const std::string& result) mutable {
        p(result);
    };
    const Ssid ssid2{"Columba"};
    manager.getExecuter(ssid2).process("AsDfGh", handler2);

    ioContext.run();
    manager.stopAll();

    std::string result1 = future1.get();
    std::string result2 = future2.get();

    EXPECT_TRUE(boost::algorithm::starts_with(result1, "AsDfGh"));
    EXPECT_TRUE(boost::algorithm::ends_with(result1, "[Cygnus-0]"));

    EXPECT_TRUE(boost::algorithm::starts_with(result2, "aSdFgH"));
    EXPECT_TRUE(boost::algorithm::ends_with(result2, "[Columba-0]"));
}

TEST(Manager, unexistsGroup) {
    Manager manager{1, logger};
    Ssid ssid{""};
    EXPECT_ANY_THROW(manager.stop(ssid));
}
