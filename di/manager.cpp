#include "manager.h"

#include <cassert>
#include <stdexcept>


Manager::Manager(std::size_t groupSize, Logger& logger)
    :
      groupSize{groupSize},
      logger{logger}
{}

Executer& Manager::getExecuter(const Ssid& ssid) {
    auto it = groups.find(ssid);
    if (it == std::end(groups)) {
        auto group = std::make_unique<Group>(ssid, groupSize, logger);
        auto pair = std::make_pair(ssid, std::move(group));
        it = groups.insert(std::move(pair)).first;
    }

    return it->second->getExecuter();
}

void Manager::stop(const Ssid& ssid) {
    auto it = groups.find(ssid);
    if (it == std::end(groups)) {
        throw std::logic_error{"Unexists group"};
    }

    it->second->stopAll();
}

void Manager::stopAll() {
    for (auto& p : groups) {
        p.second->stopAll();
    }
}
