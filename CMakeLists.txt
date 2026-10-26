cmake_minimum_required(VERSION 3.16)
project(XXProductLineProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Always emit compile_commands.json so IntelliSense and clangd find
# generated headers (protobuf/gRPC codegen under build/).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------------
# Helper: suppress all compiler warnings for every target defined under an
# external subdirectory.  Call this after add_subdirectory(external/...).
# Only non-INTERFACE targets receive compile options; INTERFACE targets have
# no translation unit of their own so -w would be silently ignored anyway.
# --------------------------------------------------------------------------
function(disable_warnings_for_directory dir)
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(_sub IN LISTS _subdirs)
        disable_warnings_for_directory("${_sub}")
    endforeach()
    get_property(_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_tgt IN LISTS _targets)
        get_target_property(_type "${_tgt}" TYPE)
        if(_type STREQUAL "STATIC_LIBRARY"
                OR _type STREQUAL "SHARED_LIBRARY"
                OR _type STREQUAL "MODULE_LIBRARY"
                OR _type STREQUAL "OBJECT_LIBRARY"
                OR _type STREQUAL "EXECUTABLE")
            target_compile_options("${_tgt}" PRIVATE -w)
        endif()
    endforeach()
endfunction()

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
# gRPC (vendored submodule) — provides grpc++, libprotobuf, protoc,
# grpc_cpp_plugin used by libs/pl_proto for code generation.
# EXCLUDE_FROM_ALL keeps unused gRPC targets out of the default build.
# --------------------------------------------------------------------------
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_CSHARP_EXT OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
# Build gRPC/protobuf/abseil as shared libs: asset libraries are SHARED and
# all link the gRPC runtime — a static runtime would be duplicated in every
# dylib and abort at startup with duplicate metric/descriptor registration.
set(BUILD_SHARED_LIBS_SAVED ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS ON)
add_subdirectory(external/grpc EXCLUDE_FROM_ALL)
disable_warnings_for_directory(${CMAKE_CURRENT_SOURCE_DIR}/external/grpc)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_SAVED})

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
    disable_warnings_for_directory(${CMAKE_CURRENT_SOURCE_DIR}/external/gtest)
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
