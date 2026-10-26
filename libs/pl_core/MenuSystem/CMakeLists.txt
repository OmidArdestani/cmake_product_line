find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets Svg)

add_library(pl_core_gui STATIC
    Ids.h
    MenuDefinitions.h
    MenuRegistry.h
    MenuRegistry.cpp
    MenuSystem.h
    Icons/IconProvider.h
    Icons/IconProvider.cpp
)
target_include_directories(pl_core_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(pl_core_gui PUBLIC Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_compile_features(pl_core_gui PUBLIC cxx_std_17)
