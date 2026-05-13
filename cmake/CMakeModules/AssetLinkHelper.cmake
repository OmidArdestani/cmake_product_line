# AssetLinkHelper.cmake
#
# Provides two reusable macros for registering asset libraries so that the
# per-asset MSVC/non-MSVC path logic does not have to be repeated by every
# caller.
#
# Usage:
#   pl_link_unique_asset(UniqueAsset1 UAsset1)
#   pl_link_shared_assets()
#
# Each macro appends to the following parent-scope lists:
#   LIBS              - linker flags / library paths
#   DEFINES           - compile-time preprocessor defines  (unique assets only)
#   ASSETS_HEADER_DIR - additional include directories
#   ASSETS_LIB_PATH   - full DLL paths for post-build copy (unique assets only)
#   Dependencies      - CMake target names for add_dependencies()

# Registers one UniqueAsset shared library.
# FULL_NAME  : CMake target name and directory name  (e.g. UniqueAsset1)
# SHORT_NAME : Preprocessor define name              (e.g. UAsset1)
macro(pl_link_unique_asset FULL_NAME SHORT_NAME)
    if(MSVC)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/release/ -l${FULL_NAME})
            list(APPEND ASSETS_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/release/lib${FULL_NAME}.dll)
        else()
            list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/debug/ -l${FULL_NAME})
            list(APPEND ASSETS_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/debug/lib${FULL_NAME}.dll)
        endif()
    else()
        list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/ -l${FULL_NAME})
        list(APPEND ASSETS_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/../${FULL_NAME}/lib${FULL_NAME}.dll)
    endif()

    list(APPEND DEFINES           -D${SHORT_NAME})
    list(APPEND ASSETS_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${FULL_NAME})
    list(APPEND Dependencies      ${FULL_NAME})
endmacro()

# Registers the SharedAssets static library.
macro(pl_link_shared_assets)
    if(MSVC)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../SharedAssets/release/ -lSharedAssets)
        else()
            list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../SharedAssets/debug/ -lSharedAssets)
        endif()
    else()
        list(APPEND LIBS -L${CMAKE_CURRENT_BINARY_DIR}/../SharedAssets/ -lSharedAssets)
    endif()

    list(APPEND ASSETS_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../SharedAssets)
    list(APPEND Dependencies      SharedAssets)
endmacro()
