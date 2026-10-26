#include "Product3Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset3.h>
#include <uniqueasset4.h>

bool Product3Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_3 ships UniqueAsset2 + UniqueAsset3 + UniqueAsset4.
    auto* asset2 = new UniqueAsset2();
    auto* asset3 = new UniqueAsset3();
    auto* asset4 = new UniqueAsset4();
    refMap.insert(asset2->assetName(), asset2);
    refMap.insert(asset3->assetName(), asset3);
    refMap.insert(asset4->assetName(), asset4);
    return true;
}
