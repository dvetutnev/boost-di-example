add_executable(legacy main.cpp)
target_link_libraries(legacy Boost::headers Threads::Threads)

add_executable(test_legacy
    test_async_result.cpp
    executer.cpp
    test_executer.cpp)
target_link_libraries(test_legacy Boost::headers GTest::gtest_main)
add_test(NAME test_legacy COMMAND test_legacy)
