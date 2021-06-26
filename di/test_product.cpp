#include "manager.h"
#include "async_result.h"

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
        auto groupSize = injector.template create<GroupSize>();
        auto factory = injector.template create<std::shared_ptr<IFactoryExecuter>>();
        return std::make_unique<Group>(std::move(ssid), groupSize, std::move(factory));
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

        di::bind<GroupSize>().to(GroupSize{2})
    );
};


} // Anonymous namespace


TEST(Product, _) {
    auto injector = di::make_injector(config());
    auto manager = injector.create<std::unique_ptr<Manager>>();

    IoContextWrapper ioContext;

    auto [promise1, future1] = AsyncResult::create(ioContext);
    auto handler1 = [p = promise1](const std::string& result) mutable {
        p(result);
    };
    manager->getExecuter(Ssid{"Cygnus"}).process("aSdFgH", handler1);

    auto [promise2, future2] = AsyncResult::create(ioContext);
    auto handler2 = [p = promise2](const std::string& result) mutable {
        p(result);
    };
    manager->getExecuter(Ssid{"Columba"}).process("AsDfGh", handler2);

    ioContext.run();
    manager->stopAll();

    std::string result1 = future1.get();
    std::string result2 = future2.get();

    EXPECT_TRUE(boost::algorithm::starts_with(result1, "AsDfGh"));
    EXPECT_TRUE(boost::algorithm::ends_with(result1, "[Cygnus-0]"));

    EXPECT_TRUE(boost::algorithm::starts_with(result2, "aSdFgH"));
    EXPECT_TRUE(boost::algorithm::ends_with(result2, "[Columba-0]"));
}
