#include "async_result.h"

#include <gtest/gtest.h>
#include <thread>


TEST(AsyncResult, simple) {
    IoContextWrapper ioContext;
    auto [promise, future] = AsyncPromise::create(ioContext);

    promise("rESULT");
    ioContext.run();

    ASSERT_EQ(future.get(), "rESULT");
}

TEST(AsyncResult, thread) {
    IoContextWrapper ioContext;
    auto [promise, future] = AsyncPromise::create(ioContext);

    auto worker = [promise = promise]() mutable {
        promise("ReSuLt");
    };
    std::thread thread{worker};

    ioContext.run();
    thread.join();

    ASSERT_EQ(future.get(), "ReSuLt");
}

TEST(AsyncResult, threads) {
    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncPromise::create(ioContext);
    auto [promise2, future2] = AsyncPromise::create(ioContext);

    auto worker1 = [promise = promise1]() mutable {
        promise("result1");
    };
    std::thread thread1{worker1};

    auto worker2 = [promise = promise2]() mutable {
        promise("result2");
    };
    std::thread thread2{worker2};

    ioContext.run();
    thread1.join();
    thread2.join();

    ASSERT_EQ(future1.get(), "result1");
    ASSERT_EQ(future2.get(), "result2");
}
