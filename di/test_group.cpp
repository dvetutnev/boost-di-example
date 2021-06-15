#include "group.h"

#include "mocks.h"
#include "async_result.h"

#include <gtest/gtest.h>
#include <boost/algorithm/string/predicate.hpp>


using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::ByMove;
using ::testing::InSequence;

namespace di = boost::di;


namespace {

const auto ssid = Ssid{"Casiopea"};

auto config = []() {
    return make_injector(
        di::bind<IFactoryExecuter>().to<MockFactoryExecuter>().in(di::singleton),
        di::bind<IGroup>().to<Group>(),

        di::bind<Ssid>().to(ssid),
        di::bind<std::size_t>().to(std::size_t{2})
    );
};

} // Anonymous namespace


TEST(Group, severalId) {
    auto injector = di::make_injector(config());

    auto& mock = injector.create<MockFactoryExecuter&>();
    {
        InSequence _;
        EXPECT_CALL(mock, create(ssid, Id{"0"}))
                .WillOnce(Return(ByMove(std::make_unique<MockExecuter>())))
                ;
        EXPECT_CALL(mock, create(ssid, Id{"1"}))
                .WillOnce(Return(ByMove(std::make_unique<MockExecuter>())))
                ;
    }

    auto group = injector.create<std::unique_ptr<IGroup>>();

    IExecuter& executer1 = group->getExecuter();
    IExecuter& executer2 = group->getExecuter();
}

TEST(Group, ringId) {
    auto injector = di::make_injector(config());

    auto& mock = injector.create<MockFactoryExecuter&>();
    {
        InSequence _;

        const Id id0{"0"};
        auto executer1 = std::make_unique<MockExecuter>();
        EXPECT_CALL(*executer1, getId)
                .WillRepeatedly(ReturnRef(id0))
                ;
        EXPECT_CALL(mock, create(ssid, Id{"0"}))
                .WillOnce(Return(ByMove(std::move(executer1))))
                ;

        const Id id1{"1"};
        auto executer2 = std::make_unique<MockExecuter>();
        EXPECT_CALL(*executer1, getId)
                .WillRepeatedly(ReturnRef(id1))
                ;
        EXPECT_CALL(mock, create(ssid, Id{"1"}))
                .WillOnce(Return(ByMove(std::move(executer2))))
                ;
        EXPECT_CALL(mock, create)
                .Times(0)
                ;
    }

    auto group = injector.create<std::unique_ptr<IGroup>>();

    IExecuter& _1 = group->getExecuter();
    IExecuter& _2 = group->getExecuter();

    IExecuter& executer = group->getExecuter();
    ASSERT_EQ(executer.getId(), Id{"0"});
}
