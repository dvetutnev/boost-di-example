#include "executer.h"

#include <algorithm>
#include <cctype>


Executer::Executer(Ssid ssid, Id id, std::shared_ptr<ILogger> logger)
    :
      ssid{std::move(ssid)},
      id{std::move(id)},
      logger{std::move(logger)},

      ioContext{},
      workGuard{boost::asio::make_work_guard(ioContext)},

      thread{std::thread{&Executer::worker, this}}
{}

void Executer::process(const std::string& data, std::function<void(const std::string&)> handler) {
    auto task = [data, handler, this]() {
        std::string result;
        std::transform(std::begin(data), std::end(data), std::back_inserter(result),
                       [](char c) -> char { return (std::islower(c)) ? std::toupper(c) : std::tolower(c); });

        std::string tail = '[' + this->ssid.value + '-' + this->id.value + ']';
        result += ' ' + tail;

        handler(result);

        this->logger->log("Log: " + result);
    };

    ioContext.post(task);
}

void Executer::stop() {
    workGuard.reset();
    thread.join();
}

const Id& Executer::getId() const {
    return id;
}

void Executer::worker() {
    ioContext.run();
}
