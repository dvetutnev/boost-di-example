#include "manager.h"

#include <cassert>
#include <stdexcept>


Manager::Manager(std::shared_ptr<IFactoryGroup> f)
    :
      factory{std::move(f)}
{}

IExecuter& Manager::getExecuter(const Ssid& ssid) {
    auto it = groups.find(ssid);
    if (it == std::end(groups)) {
        auto group = factory->create(Ssid{ssid});
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
