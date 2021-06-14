#include "group.h"


Group::Group(const Ssid& ssid, std::size_t maxSize, std::shared_ptr<ILogger> logger)
    :
      ssid{ssid},
      maxSize{maxSize},
      logger{std::move(logger)},

      currentId{0},

      instances{},
      it{std::begin(instances)}
{}

IExecuter& Group::getExecuter() {
    if (instances.size() < maxSize) {
        auto executer = std::make_unique<Executer>(ssid, Id{std::to_string(currentId)}, logger);
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
