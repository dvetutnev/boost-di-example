add_executable(test_legacy
    executer.cpp
    test_executer.cpp
    group.cpp
    test_group.cpp
    manager.cpp
    test_manager.cpp)
target_link_libraries(test_legacy Boost::headers GTest::gtest_main)
add_test(NAME test_legacy COMMAND test_legacy)
