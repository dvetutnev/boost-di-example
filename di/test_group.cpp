#include "group.h"
#include "async_result.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


namespace {

auto logger = std::make_shared<Logger>();

} // Anonymous namespace


TEST(Group, severalId) {
    Group group{Ssid{"Casiopea"}, 2, logger};

    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncResult::create(ioContext);
    auto handler1 = [p = promise1](const std::string& result) mutable {
        p(result);
    };
    group.getExecuter().process("Rtyuio", handler1);

    auto [promise2, future2] = AsyncResult::create(ioContext);
    auto handler2 = [p = promise2](const std::string& result) mutable {
        p(result);
    };
    group.getExecuter().process("Rtyuio", handler2);

    ioContext.run();
    group.stopAll();

    std::string result1 = future1.get();
    std::string result2 = future2.get();

    EXPECT_TRUE(boost::algorithm::starts_with(result1, "rTYUIO"));
    EXPECT_TRUE(boost::algorithm::ends_with(result1, "[Casiopea-0]"));

    EXPECT_TRUE(boost::algorithm::starts_with(result2, "rTYUIO"));
    EXPECT_TRUE(boost::algorithm::ends_with(result2, "[Casiopea-1]"));
}

TEST(Group, ringId) {
    Group group{Ssid{"Fornax"}, 2, logger};

    IoContextWrapper ioContext;

    IExecuter& dummy1 = group.getExecuter();
    IExecuter& dummy2 = group.getExecuter();

    IExecuter& executer = group.getExecuter();

    auto [promise, future] = AsyncResult::create(ioContext);
    auto handler = [p = promise](const std::string& result) mutable {
        p(result);
    };
    executer.process("zxCvBnm", handler);

    ioContext.run();
    group.stopAll();

    std::string result = future.get();

    EXPECT_TRUE(boost::algorithm::ends_with(result, "[Fornax-0]"));
    EXPECT_TRUE(boost::algorithm::starts_with(result, "ZXcVbNM"));
}
