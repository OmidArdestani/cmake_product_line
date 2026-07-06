#include "Product3Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset3.h>
#include <uniqueasset4.h>

Product3Builder::Product3Builder()
    : IProductBuilder()
{

}

bool Product3Builder::buildMenu(QMenuBar *menuBar)
{
    QMenu *product3Menu = menuBar->addMenu("Product 3");
    product3Menu->addAction("Action 1 for Product 3", this, SLOT(onProduct3MenuActionTriggered()));
    product3Menu->addAction("Action 2 for Product 3", this, SLOT(onProduct3MenuActionTriggered()));
    return true;
}

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

bool Product3Builder::decorate(QWidget *mainWidget)
{
    return true;
}

void Product3Builder::onProduct3MenuActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString actionText = action->text();
        qDebug() << "Triggered action for Product 1: " << actionText;
        // You can now use the actionText as needed
    }
}
