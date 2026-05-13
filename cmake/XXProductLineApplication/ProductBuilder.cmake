include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules/AssetLinkHelper.cmake)

# ProductBuilderFactory is always compiled (it contains the #ifdef chains
# so no other file needs to).
set(PRODUCT_BUILDER_HEADERS
    ProductBuilder/ProductBuilderFactory.h
)
set(PRODUCT_BUILDER_SOURCES
    ProductBuilder/ProductBuilderFactory.cpp
)

# Add the concrete builder for the active product configuration.
if(CURRENT_PRODUCT STREQUAL "Product_1")
    list(APPEND PRODUCT_BUILDER_HEADERS ProductBuilder/Product1Builder.h)
    list(APPEND PRODUCT_BUILDER_SOURCES ProductBuilder/Product1Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_2")
    list(APPEND PRODUCT_BUILDER_HEADERS ProductBuilder/Product2Builder.h)
    list(APPEND PRODUCT_BUILDER_SOURCES ProductBuilder/Product2Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_3")
    list(APPEND PRODUCT_BUILDER_HEADERS ProductBuilder/Product3Builder.h)
    list(APPEND PRODUCT_BUILDER_SOURCES ProductBuilder/Product3Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_4")
    list(APPEND PRODUCT_BUILDER_HEADERS ProductBuilder/Product4Builder.h)
    list(APPEND PRODUCT_BUILDER_SOURCES ProductBuilder/Product4Builder.cpp)
elseif(CURRENT_PRODUCT STREQUAL "Product_5")
    list(APPEND PRODUCT_BUILDER_HEADERS ProductBuilder/Product5Builder.h)
    list(APPEND PRODUCT_BUILDER_SOURCES ProductBuilder/Product5Builder.cpp)
endif()

# Always link SharedAssets.
pl_link_shared_assets()

# Link the UniqueAsset libraries selected for the current product.
foreach(ASSET IN LISTS CONFIG)
    if(${ASSET} STREQUAL "UAsset1")
        pl_link_unique_asset(UniqueAsset1 UAsset1)
    elseif(${ASSET} STREQUAL "UAsset2")
        pl_link_unique_asset(UniqueAsset2 UAsset2)
    elseif(${ASSET} STREQUAL "UAsset3")
        pl_link_unique_asset(UniqueAsset3 UAsset3)
    elseif(${ASSET} STREQUAL "UAsset4")
        pl_link_unique_asset(UniqueAsset4 UAsset4)
    endif()
endforeach()
