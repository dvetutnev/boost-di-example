#include "executer.h"
#include "async_result.h"

#include <gtest/gtest.h>


TEST(Executer, _) {
    const Ssid ssid{"sSid"};
    const Id id{"157"};

    Executer executer{ssid, id};

    IoContextWrapper ioContext;
    auto [promise, future] = AsyncResult::create(ioContext);
    auto cb = [promise = promise] (const std::string& result) mutable {
        promise(result);
    };

    executer.process("AbCd", cb);

    ioContext.run();

    std::string result = future.get();
    executer.stop();

    ASSERT_EQ(result, "aBcD [sSid-157]");
}
