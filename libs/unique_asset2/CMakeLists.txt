cmake_minimum_required(VERSION 3.16)
project(UniqueAsset2)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)

add_library(UniqueAsset2 SHARED
    src/uniqueasset2.cpp
)

target_compile_definitions(UniqueAsset2 PRIVATE UNIQUEASSET2_LIBRARY)

target_include_directories(UniqueAsset2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UniqueAsset2 PUBLIC
    pl_core
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Network
)

set_target_properties(UniqueAsset2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
