cmake_minimum_required(VERSION 3.16)
project(UniqueAsset3)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(UniqueAsset3 SHARED
    src/uniqueasset3.cpp
    src/uniqueasset3service.cpp
)

target_compile_definitions(UniqueAsset3 PRIVATE UNIQUEASSET3_LIBRARY)

target_include_directories(UniqueAsset3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UniqueAsset3 PUBLIC
    pl_core
    pl_proto
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(UniqueAsset3 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
