#include "Product1Builder.h"

#include <uniqueasset1.h>
#include <uniqueasset2.h>

bool Product1Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_1 ships UniqueAsset1 + UniqueAsset2 (see cmake/ProductConfig.cmake).
    auto* asset1 = new UniqueAsset1();
    auto* asset2 = new UniqueAsset2();
    refMap.insert(asset1->assetName(), asset1);
    refMap.insert(asset2->assetName(), asset2);
    return true;
}
