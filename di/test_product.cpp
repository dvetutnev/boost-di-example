#include "logger.h"
#include "executer.h"
#include "group.h"
#include "manager.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


namespace di = boost::di;


namespace {


using FactoryExecuter = di::extension::factory<Executer>;


template <typename TInjector>
struct FactoryGroupImpl : IFactoryGroup
{
    FactoryGroupImpl(const TInjector& i) : injector{const_cast<TInjector&>(i)} {}

    std::unique_ptr<IGroup> create(Ssid&& ssid) const override {
        auto e = di::make_injector(
                    di::extension::make_extensible(injector),
                    di::bind<Ssid>().to(std::move(ssid)) [di::override]
        );
        return nullptr;
    }

    TInjector& injector;
};

struct FactoryGroup {
  template <class TInjector, class TDependency>
  std::shared_ptr<FactoryGroupImpl<TInjector>> operator()(const TInjector& injector, const TDependency&) const {
    return std::make_shared<FactoryGroupImpl<TInjector>>(injector);
  }
};


auto config = []() {
    return di::make_injector(
        di::bind<ILogger>.to<Logger>().in(di::singleton),

        di::bind<IFactoryExecuter>.to<>(FactoryExecuter{}),
        di::bind<IFactoryGroup>().to<>(FactoryGroup{}),

        di::bind<Manager>().to<Manager>(),

        di::bind<GroupSize>().to(GroupSize{2})
    );
};


} // Anonymous namespace


TEST(Product, _) {
    auto injector = di::make_injector(config());
    auto factoryExecuter = injector.create<std::shared_ptr<IFactoryExecuter>>();
    auto factoryGroup = injector.create<std::shared_ptr<IFactoryGroup>>();
    auto manager = injector.create<std::unique_ptr<Manager>>();
}
