#include "Product4Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset4.h>

bool Product4Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_4 ships UniqueAsset2 + UniqueAsset4.
    auto* asset2 = new UniqueAsset2();
    auto* asset4 = new UniqueAsset4();
    refMap.insert(asset2->assetName(), asset2);
    refMap.insert(asset4->assetName(), asset4);
    return true;
}
