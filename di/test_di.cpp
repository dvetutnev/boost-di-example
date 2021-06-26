#include "mocks.h"


using ::testing::Return;
using ::testing::ByMove;


namespace di = boost::di;


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

TEST(DI, factory) {
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
    std::size_t method() const override { return value * 2; }

    std::size_t value;
};

using IFactoryFoo = di::extension::ifactory<IFoo, Value>;
using FactoryFoo = di::extension::factory<Foo>;


} // Anonymous namespace

TEST(DI, factory_with_arg) {
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


namespace {


struct FooWithInject : Foo
{
    FooWithInject(Value p, std::shared_ptr<ILogger> l)
        :
          Foo{p},
          logger{std::move(l)}
    {}

    std::size_t method() const override {
        std::size_t result = Foo::method();
        logger->log(std::to_string(result));
        return result;
    }

    std::shared_ptr<ILogger> logger;
};

using IFactoryFooWithInject = di::extension::ifactory<IFoo, Value>;
using FactoryFooWithInject = di::extension::factory<FooWithInject>;


} // Anonymous namespace

TEST(DI, factory_with_inject) {
    auto injector = di::make_injector(
        di::bind<ILogger>().to<MockLogger>().in(di::singleton),
        di::bind<IFactoryFooWithInject>().to(FactoryFooWithInject{})
    );

    auto& logger = injector.create<MockLogger&>();
    EXPECT_CALL(logger, log).Times(1);

    auto factory = injector.create<std::shared_ptr<IFactoryFooWithInject>>();
    auto foo = factory->create(Value{111});
    EXPECT_EQ(foo->method(), 222);
}

TEST(DI, factory_with_inject2) {
    auto injector = di::make_injector(
        di::bind<ILogger>().to<Logger>().in(di::singleton),
        di::bind<IFactoryFooWithInject>().to(FactoryFooWithInject{})
    );

    auto factory = injector.create<std::shared_ptr<IFactoryFooWithInject>>();
    auto foo = factory->create(Value{111});
    EXPECT_EQ(foo->method(), 222);
}


namespace {


struct FooWithFactoryBar : Foo
{
    FooWithFactoryBar(Value p, std::shared_ptr<IFactoryBar> f)
        :
          Foo{p},
          factory{std::move(f)}
    {}

    std::size_t method() const override {
        std::unique_ptr bar = factory->create();
        std::size_t result = Foo::method() + bar->method();
        return result;
    }

    std::shared_ptr<IFactoryBar> factory;
};


} // Anonymous namespace

TEST(DI, inject_factory) {
    auto injector = di::make_injector(
        di::bind<IFactoryBar>().to(FactoryBar{}),
        di::bind<Value>().to(Value{3})
    );
    auto foo = injector.create<std::unique_ptr<FooWithFactoryBar>>();
    ASSERT_EQ(foo->method(), 3 * 2 + 42);
}

TEST(DI, inject_factory_einjector) {
    auto injector = di::make_injector(
        di::bind<IFactoryBar>().to(FactoryBar{}),
        di::bind<Value>().to(Value{3})
    );
    auto eInjector = di::make_injector(
        di::extension::make_extensible(injector),
        di::bind<Value>().to(Value{5}) [di::override]
    );
    auto foo = eInjector.create<std::unique_ptr<FooWithFactoryBar>>();
    ASSERT_EQ(foo->method(), 5 * 2 + 42);
}
