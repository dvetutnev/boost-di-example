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
    EXPECT_CALL(*mock, create(Ssid{ssid}, Id{id}))
            .WillOnce(Return(ByMove(std::move(executer_))))
            ;

    auto factory = injector.create<std::shared_ptr<IFactoryExecuter>>();
    std::unique_ptr<IExecuter> executer = factory->create(Ssid{ssid}, Id{id});

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
    auto factory = injector.create<std::shared_ptr<IFactoryBar>>();
    auto bar = factory->create();
    ASSERT_EQ(bar->method(), 42);
}


namespace {


struct IFoo
{
    virtual std::size_t method() const = 0;
    virtual ~IFoo() = default;
};

struct Value
{
    std::size_t value;
};

struct Foo : IFoo
{
    Foo(const Value& p) : value{p.value} {}
    virtual std::size_t method() const override { return value * 2; }

    std::size_t value;
};

using IFactoryFoo = di::extension::ifactory<IFoo, Value>;
using FactoryFoo = di::extension::factory<Foo>;


} // Anonymous namespace

TEST(DI, factory_product_with_arg) {
    auto injector = di::make_injector(
        di::bind<IFactoryFoo>().to(FactoryFoo{})
    );
    auto factory = injector.create<std::shared_ptr<IFactoryFoo>>();
    auto foo = factory->create(Value{13});
    ASSERT_EQ(foo->method(), 26);
}


namespace {


struct MockFactoryFoo : IFactoryFoo
{
    MOCK_METHOD(std::unique_ptr<IFoo>, create, (Value&&), (const, override));
};


} // Anonymous namespace

TEST(DI, factory_mock) {
    auto injector = di::make_injector(
        di::bind<IFactoryFoo>().to<MockFactoryFoo>()
    );
    auto factory = injector.create<std::shared_ptr<IFactoryFoo>>();
    ASSERT_TRUE(factory);
}
