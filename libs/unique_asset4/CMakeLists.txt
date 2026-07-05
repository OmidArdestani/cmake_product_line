cmake_minimum_required(VERSION 3.16)
project(UniqueAsset4)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(UniqueAsset4 SHARED
    src/uniqueasset4.cpp
    src/uniqueasset4service.cpp
)

target_compile_definitions(UniqueAsset4 PRIVATE UNIQUEASSET4_LIBRARY)

target_include_directories(UniqueAsset4 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UniqueAsset4 PUBLIC
    pl_core
    pl_proto
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(UniqueAsset4 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
