cmake_minimum_required(VERSION 3.16)
project(XXProductLineProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------
# Output directories — shared libs and the app binary land in the same bin/
# so the OS dynamic linker / Qt can find them at runtime without rpath trickery.
# --------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --------------------------------------------------------------------------
# Qt: single top-level find_package so all sub-targets share the result
# --------------------------------------------------------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets WebSockets Network)

# --------------------------------------------------------------------------
# GoogleTest (only when testing is enabled)
# --------------------------------------------------------------------------
option(BUILD_TESTING "Build the test suite" ON)
if(BUILD_TESTING)
    # Recommended gtest settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/gtest)
    enable_testing()
endif()

# --------------------------------------------------------------------------
# Sub-projects
# --------------------------------------------------------------------------
add_subdirectory(libs)
add_subdirectory(app)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
