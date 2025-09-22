# Define the project and specify C++ standard
cmake_minimum_required(VERSION 3.16)
project(XXProductLineApplication VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network)

# Find gRPC and Protocol Buffers
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)
pkg_check_modules(PROTOBUF REQUIRED protobuf)

# Set the current product
set(CURRENT_PRODUCT "Product_1")

# Handle the ProductLineAPI subdirectory
include(ProductLineAPI/ProductLineAPI.cmake)
include(ProductConfig.cmake)
include(ProductBuilder.cmake)

# Add the executable target
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${PRODUCT_LINE_API_SOURCES}
        ${PRODUCT_LINE_API_HEADERS}
        ${PRODUCT_BUILDER_HEADERS}
        ${PRODUCT_BUILDER_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(XXProductLineApplication
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET XXProductLineApplication APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(XXProductLineApplication SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(XXProductLineApplication
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Apply the DEFINES to the target after it is defined
target_compile_definitions(XXProductLineApplication PRIVATE ${DEFINES})

# Apply dependencies
add_dependencies(XXProductLineApplication ${Dependencies})

# Link the required Qt6/5 libraries and gRPC
target_link_libraries(XXProductLineApplication PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    ${GRPC_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${LIBS}
)

# Include directories for headers
target_include_directories(XXProductLineApplication PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI
    ${GRPC_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${ASSETS_HEADER_DIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(XXProductLineApplication)
endif()

