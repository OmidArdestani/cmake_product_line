cmake_minimum_required(VERSION 3.16)
project(XXProductLineApplication VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load product configuration (sets CURRENT_PRODUCT cache var + PRODUCT_ASSETS list)
include(${CMAKE_SOURCE_DIR}/cmake/ProductConfig.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ProductLineAPI/productlineapi.h
    ProductLineAPI/productlineapi.cpp
    ProductLineAPI/WebSocketsAPI.h
    ProductLineAPI/websocketsapi.cpp
)

# Product-specific builder source
if(CURRENT_PRODUCT STREQUAL "Product_1")
    list(APPEND PROJECT_SOURCES
        ProductBuilder/Product1Builder.h
        ProductBuilder/Product1Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_2")
    list(APPEND PROJECT_SOURCES
        ProductBuilder/Product2Builder.h
        ProductBuilder/Product2Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_3")
    list(APPEND PROJECT_SOURCES
        ProductBuilder/Product3Builder.h
        ProductBuilder/Product3Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_4")
    list(APPEND PROJECT_SOURCES
        ProductBuilder/Product4Builder.h
        ProductBuilder/Product4Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_5")
    list(APPEND PROJECT_SOURCES
        ProductBuilder/Product5Builder.h
        ProductBuilder/Product5Builder.cpp)
endif()

# --------------------------------------------------------------------------
# Executable target
# --------------------------------------------------------------------------
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(XXProductLineApplication
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(XXProductLineApplication SHARED ${PROJECT_SOURCES})
    else()
        add_executable(XXProductLineApplication ${PROJECT_SOURCES})
    endif()
endif()

# --------------------------------------------------------------------------
# Product define (e.g. -DProduct_1)
# --------------------------------------------------------------------------
target_compile_definitions(XXProductLineApplication PRIVATE ${CURRENT_PRODUCT})

# --------------------------------------------------------------------------
# Libraries to link based on active product assets
# --------------------------------------------------------------------------
set(ASSET_LIBS pl_core)

foreach(ASSET IN LISTS PRODUCT_ASSETS)
    if(ASSET STREQUAL "UAsset1")
        list(APPEND ASSET_LIBS UniqueAsset1)
        target_compile_definitions(XXProductLineApplication PRIVATE UAsset1)
    elseif(ASSET STREQUAL "UAsset2")
        list(APPEND ASSET_LIBS UniqueAsset2)
        target_compile_definitions(XXProductLineApplication PRIVATE UAsset2)
    elseif(ASSET STREQUAL "UAsset3")
        list(APPEND ASSET_LIBS UniqueAsset3)
        target_compile_definitions(XXProductLineApplication PRIVATE UAsset3)
    elseif(ASSET STREQUAL "UAsset4")
        list(APPEND ASSET_LIBS UniqueAsset4)
        target_compile_definitions(XXProductLineApplication PRIVATE UAsset4)
    endif()
endforeach()

target_link_libraries(XXProductLineApplication PRIVATE
    ${ASSET_LIBS}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Network
)

# --------------------------------------------------------------------------
# Include directories
# ProductLineAPI/productlineapi.cpp includes "mainwindow.h" — add app/ root.
# --------------------------------------------------------------------------
target_include_directories(XXProductLineApplication PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductLineAPI
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(XXProductLineApplication)
endif()
