#include "executer.h"

#include <algorithm>
#include <cctype>


Executer::Executer(const Ssid& ssid, const Id& id)
    :
      ssid{ssid},
      id{id},

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
    };

    ioContext.post(task);
}

void Executer::stop() {
    workGuard.reset();
    thread.join();
}

void Executer::worker() {
    ioContext.run();
}
