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
        return asyncHandler(boost::system::error_code{}, message);
    };

    auto future = asyncResult.get();

    ioContext.post(handler);

    ioContext.run();

    ASSERT_EQ(future.get(), "rEsUlT");
}
