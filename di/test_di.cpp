#include "mocks.h"


using ::testing::Return;
using ::testing::ByMove;


namespace di = boost::di;


TEST(DI, logger_sigleton) {
    auto injector = di::make_injector(
        di::bind<ILogger>().to<MockLogger>().in(di::singleton)
    );

    auto mock = injector.create<std::shared_ptr<MockLogger>>();
    EXPECT_CALL(*mock, log)
            .Times(1)
            ;


    auto logger = injector.create<std::shared_ptr<ILogger>>();
    logger->log("zzzzzzzzzzzzzzzZZ");

    ASSERT_EQ(logger, mock);
}

TEST(DI, factory_singleton) {
    auto injector = di::make_injector(
        di::bind<IFactoryExecuter>().to<MockFactoryExecuter>().in(di::singleton)
    );

    const Ssid ssid{"Cepheus"};
    const Id id{"42"};

    auto executer_ = std::make_unique<MockExecuter>();
    auto executerRaw = executer_.get();

    auto mock = injector.create<std::shared_ptr<MockFactoryExecuter>>();
    EXPECT_CALL(*mock, create(ssid, id))
            .WillOnce(Return(ByMove(std::move(executer_))))
            ;

    auto factory = injector.create<std::shared_ptr<IFactoryExecuter>>();
    std::unique_ptr<IExecuter> executer = factory->create(ssid, id);

    ASSERT_EQ(executer.get(), executerRaw);
}


namespace {


struct IBar
{
    virtual std::size_t method() const = 0;
    virtual ~IBar() = default;
};

struct Bar : IBar
{
    std::size_t method() const override { return 42; }
};

using IFactoryBar = di::extension::ifactory<IBar>;
using FactoryBar = di::extension::factory<Bar>;


} // Anonymouns namespace


TEST(DI, factory_product) {
    auto injector = di::make_injector(
        di::bind<IFactoryBar>().to(FactoryBar{})
    );
    auto& factory = injector.create<IFactoryBar&>();
    auto bar = factory.create();
}
