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
    test_grpc_api.cpp
)

target_link_libraries(XXProductLineTests PRIVATE
    GTest::gtest_main
    UniqueAsset1
    UniqueAsset2
    UniqueAsset3
    UniqueAsset4
    pl_proto
    Qt${QT_VERSION_MAJOR}::Core
)

# Ensure shared libs are in the same output dir so the test binary finds them at runtime
set_target_properties(XXProductLineTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# PRE_TEST: discovery runs at ctest time, not as POST_BUILD step —
# avoids deleting the binary when discovery hiccups and keeps builds green.
gtest_discover_tests(XXProductLineTests DISCOVERY_MODE PRE_TEST)

# GUI tests have their own QApplication and don't add Widgets to facility tests.
add_executable(MenuSystemTests gui_test_main.cpp test_menu_system.cpp test_menu_system_builder.cpp)
target_link_libraries(MenuSystemTests PRIVATE GTest::gtest pl_core_gui)
gtest_discover_tests(MenuSystemTests DISCOVERY_MODE PRE_TEST TEST_PREFIX "MenuSystem.")

# Compile the actual window, controllers and selected builder for every product.
# Function scope keeps each ProductConfig selection out of the app configuration.
function(add_product_menu_tests product)
    set(CURRENT_PRODUCT ${product})
    include(${CMAKE_SOURCE_DIR}/cmake/ProductConfig.cmake)
    string(REPLACE "Product_" "" product_number ${product})
    set(target ${product}MenuTests)
    add_executable(${target}
        gui_test_main.cpp
        test_product_menus.cpp
        ${CMAKE_SOURCE_DIR}/app/mainwindow.h
        ${CMAKE_SOURCE_DIR}/app/mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/app/mainwindow.ui
        ${CMAKE_SOURCE_DIR}/app/Menus/ApplicationMenus.cpp
        ${CMAKE_SOURCE_DIR}/app/Menus/ApplicationIcons.cpp
        ${CMAKE_SOURCE_DIR}/app/Tools/ToolController.cpp
        ${CMAKE_SOURCE_DIR}/app/ProductBuilder/Product${product_number}Builder.cpp
        ${CMAKE_SOURCE_DIR}/app/ProductLineAPI/grpcapiserver.cpp
        ${CMAKE_SOURCE_DIR}/app/ProductLineAPI/coreservice.cpp
        ${CMAKE_SOURCE_DIR}/app/resources/icons.qrc
    )
    set_target_properties(${target} PROPERTIES AUTOUIC ON AUTORCC ON)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/app)
    target_compile_definitions(${target} PRIVATE ${product} ${PRODUCT_ASSETS})
    target_link_libraries(${target} PRIVATE GTest::gtest pl_core pl_core_gui pl_proto
        Qt${QT_VERSION_MAJOR}::Network)
    foreach(asset IN LISTS PRODUCT_ASSETS)
        string(REPLACE "UAsset" "UniqueAsset" asset_target ${asset})
        target_link_libraries(${target} PRIVATE ${asset_target})
    endforeach()
    # Cold startup loads Qt plus the product's gRPC shared libraries on macOS.
    gtest_discover_tests(${target} DISCOVERY_MODE PRE_TEST DISCOVERY_TIMEOUT 60 TEST_PREFIX "${product}.")
endfunction()

foreach(product_number RANGE 1 5)
    add_product_menu_tests(Product_${product_number})
endforeach()
