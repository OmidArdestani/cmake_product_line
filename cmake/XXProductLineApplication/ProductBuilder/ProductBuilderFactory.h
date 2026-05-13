#pragma once

#include "IProductBuilder.h"

// Factory that creates the correct IProductBuilder for the active product
// configuration. The selection is driven by the same compile-time defines
// (Product_1 … Product_5) that are set by ProductConfig.cmake, so no
// caller ever needs to contain product-specific #ifdef chains.
class ProductBuilderFactory
{
public:
    // Returns a heap-allocated builder for the currently compiled product, or
    // nullptr when no product define is active. The caller is responsible for
    // the object's lifetime (typically by calling setParent() or delete).
    static IProductBuilder* create();
};
