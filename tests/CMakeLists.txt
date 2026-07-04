cmake_minimum_required(VERSION 3.16)
project(XXProductLineTests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# GoogleTest is added via the root CMakeLists (add_subdirectory(external/gtest))
include(GoogleTest)

add_executable(XXProductLineTests
    test_pl_core.cpp
    test_unique_asset1.cpp
    test_unique_asset2.cpp
    test_unique_asset3.cpp
    test_unique_asset4.cpp
)

target_link_libraries(XXProductLineTests PRIVATE
    GTest::gtest_main
    UniqueAsset1
    UniqueAsset2
    UniqueAsset3
    UniqueAsset4
    Qt${QT_VERSION_MAJOR}::Core
)

# Ensure shared libs are in the same output dir so the test binary finds them at runtime
set_target_properties(XXProductLineTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

gtest_discover_tests(XXProductLineTests)
