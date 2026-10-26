cmake_minimum_required(VERSION 3.16)
project(UniqueAsset2)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(UniqueAsset2 SHARED
    src/uniqueasset2.cpp
    src/uniqueasset2service.cpp
)

target_compile_definitions(UniqueAsset2 PRIVATE UNIQUEASSET2_LIBRARY)

target_include_directories(UniqueAsset2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UniqueAsset2 PUBLIC
    pl_core
    pl_proto
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(UniqueAsset2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
