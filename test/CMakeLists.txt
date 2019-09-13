cmake_minimum_required(VERSION 3.13)
project(MultidimensionalAdaptiveIndex_tests)
set (CMAKE_CXX_STANDARD 14)

# Catch lib for unit tests
include(ExternalProject)
ExternalProject_Add(catch2
    URL "https://github.com/catchorg/Catch2/archive/v2.9.1.tar.gz"
    URL_HASH MD5=4980778888fed635bf191d8a86f9f89c
    CMAKE_ARGS  -DCATCH_BUILD_TESTING=OFF
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/deps
)

include_directories(${CMAKE_SOURCE_DIR}/deps/include/catch2)

# Compile tests
set(TEST_SOURCES 
        ${CMAKE_SOURCE_DIR}/test/test.cpp
        ${CMAKE_SOURCE_DIR}/test/end_to_end_tests/small_test/small_test.cpp
        ${CMAKE_SOURCE_DIR}/test/indexes/index_factory.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests MDAI_lib)
add_dependencies(tests catch2)