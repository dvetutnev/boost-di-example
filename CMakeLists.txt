# Boost.DI example
# Dmitriy Vetutnev, 2021

cmake_minimum_required(VERSION 3.7)
project(boost-di-example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(conan.cmake)

conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}/conanfile.txt
    BUILD missing
    REMOTE conan-center
    SETTINGS ${settings})

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads)

enable_testing()

add_subdirectory(legacy)

