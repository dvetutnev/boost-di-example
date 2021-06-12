#include "executer.h"
#include "async_result.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


TEST(Executer, single) {
    const Ssid ssid{"sSid"};
    const Id id{"157"};

    Executer executer{ssid, id};

    IoContextWrapper ioContext;
    auto [promise, future] = AsyncResult::create(ioContext);
    auto handler = [promise = promise] (const std::string& result) mutable {
        promise(result);
    };

    executer.process("AbCd", handler);

    ioContext.run();

    std::string result = future.get();
    executer.stop();

    ASSERT_EQ(result, "aBcD [sSid-157]");
}

TEST(Executer, multiple) {
    Executer executer1{Ssid{"aaaa"}, Id{"42"}};
    Executer executer2{Ssid{"bbbb"}, Id{"43"}};
    Executer executer3{Ssid{"cccc"}, Id{"44"}};

    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncResult::create(ioContext);
    auto handler1 = [promise = promise1] (const std::string& result) mutable {
        promise(result);
    };
    auto [promise2, future2] = AsyncResult::create(ioContext);
    auto handler2 = [promise = promise2] (const std::string& result) mutable {
        promise(result);
    };
    auto [promise3, future3] = AsyncResult::create(ioContext);
    auto handler3 = [promise = promise3] (const std::string& result) mutable {
        promise(result);
    };

    executer1.process("AbCd", handler1);
    executer2.process("abcd", handler2);
    executer3.process("ABCD", handler3);

    ioContext.run();

    std::string result1 = future1.get();
    std::string result2 = future2.get();
    std::string result3 = future3.get();

    executer1.stop();
    executer2.stop();
    executer3.stop();

    EXPECT_TRUE(boost::algorithm::starts_with(result1, "aBcD"));
    EXPECT_TRUE(boost::algorithm::starts_with(result2, "ABCD"));
    EXPECT_TRUE(boost::algorithm::starts_with(result3, "abcd"));
}
