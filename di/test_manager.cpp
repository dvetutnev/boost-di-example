#include "manager.h"
#include "mocks.h"

#include <gmock/gmock.h>


using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::ByMove;


namespace di = boost::di;


namespace {

auto config = []() {
    return di::make_injector(
        di::bind<IFactoryGroup>().to<MockFactoryGroup>().in(di::singleton),
        di::bind<IGroup>().to<MockGroup>().in(di::unique),
        di::bind<IExecuter>().to<MockExecuter>(),

        di::bind<Manager>().to<Manager>()
    );
};

} // Anonymous namespace


TEST(Manager, createGroup) {
    auto injector = di::make_injector(config());


    IExecuter& executer = injector.create<IExecuter&>();


    auto group1 = injector.create<std::unique_ptr<MockGroup>>();

    EXPECT_CALL(*group1, getExecuter())
            .WillOnce(ReturnRef(executer))
            .WillOnce(ReturnRef(executer))
            ;

    auto group2 = injector.create<std::unique_ptr<MockGroup>>();

    EXPECT_CALL(*group2, getExecuter())
            .WillOnce(ReturnRef(executer))
            ;


    auto& factory = injector.create<MockFactoryGroup&>();

    EXPECT_CALL(factory, create(Ssid{"Lepus"}))
            .WillOnce(Return(ByMove(std::move(group1))))
            ;
    EXPECT_CALL(factory, create(Ssid{"Cetus"}))
            .WillOnce(Return(ByMove(std::move(group2))))
            ;


    auto manager = injector.create<std::unique_ptr<Manager>>();

    IExecuter& _1 = manager->getExecuter(Ssid{"Lepus"});
    IExecuter& _2 = manager->getExecuter(Ssid{"Cetus"});
    IExecuter& _3 = manager->getExecuter(Ssid{"Lepus"});
}
/*
TEST(Manager, unexistsGroup) {
    Manager manager{1, logger};
    Ssid ssid{""};
    EXPECT_ANY_THROW(manager.stop(ssid));
}
*/
