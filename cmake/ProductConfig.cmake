# cmake/ProductConfig.cmake
# Sets the CONFIG list (which UniqueAssets to include) and defines the product macro.
# Consumed by app/CMakeLists.txt.
#
# Usage:  cmake -DCURRENT_PRODUCT=Product_2 ..
#
# Supported values: Product_1 .. Product_5

set(CURRENT_PRODUCT "Product_1" CACHE STRING "Active product variant (Product_1 .. Product_5)")
set_property(CACHE CURRENT_PRODUCT PROPERTY STRINGS Product_1 Product_2 Product_3 Product_4 Product_5)

if(CURRENT_PRODUCT STREQUAL "Product_1")
    set(PRODUCT_ASSETS UAsset1 UAsset2)

elseif(CURRENT_PRODUCT STREQUAL "Product_2")
    set(PRODUCT_ASSETS UAsset2 UAsset3)

elseif(CURRENT_PRODUCT STREQUAL "Product_3")
    set(PRODUCT_ASSETS UAsset2 UAsset3 UAsset4)

elseif(CURRENT_PRODUCT STREQUAL "Product_4")
    set(PRODUCT_ASSETS UAsset2 UAsset4)

elseif(CURRENT_PRODUCT STREQUAL "Product_5")
    set(PRODUCT_ASSETS UAsset1 UAsset3 UAsset4)

else()
    message(FATAL_ERROR "Unknown CURRENT_PRODUCT value: ${CURRENT_PRODUCT}. "
                        "Valid values: Product_1 Product_2 Product_3 Product_4 Product_5")
endif()

message(STATUS "Building product: ${CURRENT_PRODUCT}  (assets: ${PRODUCT_ASSETS})")
