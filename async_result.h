#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/use_future.hpp>

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

struct AsyncResult
{
    using Signature = void (boost::system::error_code, const std::string&);
    using CompletionToken = std::decay_t<decltype(boost::asio::use_future)>;

    using Result = boost::asio::async_result<CompletionToken, Signature>;
    using Handler = Result::completion_handler_type;

    static auto create(IoContextWrapper& ioContext) {
        Handler asyncHandler{boost::asio::use_future};
        Result asyncResult{asyncHandler};

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
