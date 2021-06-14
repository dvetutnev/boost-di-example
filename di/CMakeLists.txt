add_executable(test_di
    executer.cpp
    test_executer.cpp
    group.cpp
    test_group.cpp
    manager.cpp
    test_manager.cpp)
target_link_libraries(test_di Boost::headers di::di GTest::gmock_main)
add_test(NAME test_di COMMAND test_di)
