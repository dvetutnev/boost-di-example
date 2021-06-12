#include "group.h"


Group::Group(Ssid ssid, std::size_t maxSize)
    :
      ssid{ssid},
      maxSize{maxSize},

      nextId{0}
{}

Executer& Group::getExecuter() {
    if (instances.size() < maxSize) {
        auto executer = std::make_unique<Executer>(ssid, Id{std::to_string(nextId)});
        instances.push_back(std::move(executer));
        nextId++;
    }

    return *(instances.back());
}

void Group::stopAll() {
    for (auto& i : instances) {
        i->stop();
    }
}
