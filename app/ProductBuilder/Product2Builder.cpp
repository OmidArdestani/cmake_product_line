#include "Product2Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset3.h>

bool Product2Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_2 ships UniqueAsset2 + UniqueAsset3 (see cmake/ProductConfig.cmake).
    auto* asset2 = new UniqueAsset2();
    auto* asset3 = new UniqueAsset3();
    refMap.insert(asset2->assetName(), asset2);
    refMap.insert(asset3->assetName(), asset3);
    return true;
}
