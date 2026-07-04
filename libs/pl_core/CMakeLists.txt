cmake_minimum_required(VERSION 3.16)
project(pl_core)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(pl_core STATIC
    src/sharedassets.cpp
)

target_include_directories(pl_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pl_core PUBLIC Qt${QT_VERSION_MAJOR}::Core)

set_target_properties(pl_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
