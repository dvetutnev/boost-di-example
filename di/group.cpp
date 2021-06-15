#include "group.h"


Group::Group(const Ssid& ssid, std::size_t maxSize, std::shared_ptr<IFactoryExecuter> factory)
    :
      ssid{ssid},
      maxSize{maxSize},
      factory{std::move(factory)},

      currentId{0},

      instances{},
      it{std::begin(instances)}
{}

IExecuter& Group::getExecuter() {
    if (instances.size() < maxSize) {
        auto executer = factory->create(Ssid{ssid}, Id{std::to_string(currentId)});
        currentId++;
        it = instances.insert(it, std::move(executer));
    }
    else {
        ++it;
        if (it == std::end(instances)) {
            it = std::begin(instances);
        }
    }

    return **it;
}

void Group::stopAll() {
    for (auto& i : instances) {
        i->stop();
    }
}
