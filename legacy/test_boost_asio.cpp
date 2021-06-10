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

#include <tuple>

struct AsyncPromise
{
    using Signature = void (boost::system::error_code, const std::string&);
    using CompletionToken = std::decay_t<decltype(boost::asio::use_future)>;

    using AsyncResult = boost::asio::async_result<CompletionToken, Signature>;
    using AsyncHandler = AsyncResult::completion_handler_type;

    static auto create(boost::asio::io_context& ioContext) {
        AsyncHandler asyncHandler{boost::asio::use_future};
        AsyncResult asyncResult{asyncHandler};

        auto promise = [&ioContext, asyncHandler](const std::string& message) mutable {
            auto handler = [asyncHandler, message]() mutable {
                return asyncHandler(boost::system::error_code{}, message);
            };
            ioContext.post(handler);
        };

        return std::make_tuple(promise, asyncResult.get());
    }
};

#include <thread>

TEST(AsyncResult, simple) {
    boost::asio::io_context ioContext;
    auto [promise, future] = AsyncPromise::create(ioContext);

    promise("rESULT");
    ioContext.run();

    ASSERT_EQ(future.get(), "rESULT");
}

TEST(AsyncResult, thread) {
    boost::asio::io_context ioContext;
    auto [promise, future] = AsyncPromise::create(ioContext);

    auto worker = [promise = promise]() mutable {
        promise("ReSuLt");
    };
    std::thread thread{worker};
    thread.join();

    ioContext.run();

    ASSERT_EQ(future.get(), "ReSuLt");
}
