#include "group.h"
#include "async_result.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


TEST(Group, severalId) {
    Group group{Ssid{"Casiopea"}, 2};

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
