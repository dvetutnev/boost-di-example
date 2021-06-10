#include <boost/asio/io_context.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/use_future.hpp>


#include <gtest/gtest.h>

TEST(Boost_ASIO, async_result) {
    boost::asio::io_context ioContext;

    using Sig = void (boost::system::error_code, const std::string&);
    using CompletionToken = std::decay_t<decltype(boost::asio::use_future)>;

    using AsyncResult = boost::asio::async_result<CompletionToken, Sig>;
    using AsyncHandler = AsyncResult::completion_handler_type;

    AsyncHandler asyncHandler{boost::asio::use_future};
    AsyncResult asyncResult{asyncHandler};

    std::string message = "rEsUlT";
    auto handler = [asyncHandler, message]() mutable {
        asyncHandler(boost::system::error_code{}, message);
    };

    auto future = asyncResult.get();

    ioContext.post(handler);

    ioContext.run();

    ASSERT_EQ(future.get(), "rEsUlT");
}

#include <memory>
#include <tuple>
#include <stdexcept>


class IoContextWrapper
{
public:
    void addWork() {
        workCount++;
        if (!workGuard) {
            workGuard = std::make_unique<WorkGuard>(ioContext.get_executor());
        }
    }

    void removeWork() {
        if (workCount == 0) {
            std::logic_error{"Work empty"};
        }
        workCount--;
        if (workCount == 0){
            workGuard.reset();
        }
    }

    boost::asio::io_context& get() {
        return ioContext;
    }

    void run() {
        ioContext.run();
    }

private:
    boost::asio::io_context ioContext;

    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    std::unique_ptr<WorkGuard> workGuard;

    std::size_t workCount = 0;
};

struct AsyncPromise
{
    using Signature = void (boost::system::error_code, const std::string&);
    using CompletionToken = std::decay_t<decltype(boost::asio::use_future)>;

    using AsyncResult = boost::asio::async_result<CompletionToken, Signature>;
    using AsyncHandler = AsyncResult::completion_handler_type;

    static auto create(IoContextWrapper& ioContext) {
        AsyncHandler asyncHandler{boost::asio::use_future};
        AsyncResult asyncResult{asyncHandler};

        ioContext.addWork();
        auto promise = [&ioContext, asyncHandler](const std::string& message) mutable {
            auto handler = [asyncHandler, message]() mutable {
                return asyncHandler(boost::system::error_code{}, message);
            };
            ioContext.get().post(handler);
            ioContext.removeWork();
        };

        return std::make_tuple(promise, asyncResult.get());
    }
};

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
