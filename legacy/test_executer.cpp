#include "executer.h"

#include <gtest/gtest.h>
#include <future>


TEST(Executer, _) {
    const Ssid ssid{"sSid"};
    const Id id{"157"};

    Executer executer{ssid, id};

    std::promise<std::string> promise;
    auto future = promise.get_future();
    auto handler = [&promise] (const std::string& result) {
        promise.set_value(result);
    };

    executer.process("AbCd", handler);
    std::string result = future.get();
    executer.stop();

    ASSERT_EQ(result, "aBcD [sSid-157]");
}
