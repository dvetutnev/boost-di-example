#include "executer.h"

Executer::Executer(const Ssid& ssid, const Id& id)
    :
      ssid{ssid},
      id{id},

      ioContext{},
      workGuard{boost::asio::make_work_guard(ioContext)},

      thread{std::thread{&Executer::worker, this}}
{}

void Executer::process(const std::string& data, std::function<void(const std::string&)> cb) {
    auto task = [data, cb]() {
        cb(data);
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
