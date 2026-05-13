# Define the project and specify C++ standard
cmake_minimum_required(VERSION 3.16)
project(XXProductLineTests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules/AssetLinkHelper.cmake)

include(SharedAssets/SharedAssetsTest.cmake)
include(UniqueAsset1/UniqueAsset1Test.cmake)
include(UniqueAsset2/UniqueAsset2Test.cmake)
include(UniqueAsset3/UniqueAsset3Test.cmake)
include(UniqueAsset4/UniqueAsset4Test.cmake)

# Add sources for the test executable
list(APPEND SOURCES main.cpp)
# Add the executable target
set(PROJECT_SOURCES
    main.cpp
    ${SOURCES}
    ${HEADERS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(XXProductLineTests
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET XXProductLineTests APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(XXProductLineTests SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(XXProductLineTests
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Apply dependencies
add_dependencies(XXProductLineTests
    ${Dependencies}
)

# Link the required Qt6/5 libraries
target_link_libraries(XXProductLineTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
    ${LIBS}
)

# Include directories for headers
target_include_directories(XXProductLineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ASSETS_HEADER_DIR}

)

# Enable testing with CTest
enable_testing()

# Add the test
add_test(NAME XXProductLineTests COMMAND XXProductLineTests)

foreach(DLL ${ASSETS_LIB_PATH})
    message(${DLL})
    add_custom_command(
        TARGET XXProductLineTests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL} ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()


