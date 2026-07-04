cmake_minimum_required(VERSION 3.16)
project(UniqueAsset3)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)

add_library(UniqueAsset3 SHARED
    src/uniqueasset3.cpp
)

target_compile_definitions(UniqueAsset3 PRIVATE UNIQUEASSET3_LIBRARY)

target_include_directories(UniqueAsset3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UniqueAsset3 PUBLIC
    pl_core
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Network
)

set_target_properties(UniqueAsset3 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
