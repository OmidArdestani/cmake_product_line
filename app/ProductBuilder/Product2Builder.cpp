#include "Product2Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset3.h>

Product2Builder::Product2Builder()
    : IProductBuilder()
{

}

bool Product2Builder::buildMenu(QMenuBar *menuBar)
{
    QMenu *product2Menu = menuBar->addMenu("Product 2");
    product2Menu->addAction("Action 1 for Product 2", this, SLOT(onProduct2MenuActionTriggered()));
    product2Menu->addAction("Action 2 for Product 2", this, SLOT(onProduct2MenuActionTriggered()));
    return true;
}

bool Product2Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_2 ships UniqueAsset2 + UniqueAsset3 (see cmake/ProductConfig.cmake).
    auto* asset2 = new UniqueAsset2();
    auto* asset3 = new UniqueAsset3();
    refMap.insert(asset2->assetName(), asset2);
    refMap.insert(asset3->assetName(), asset3);
    return true;
}

bool Product2Builder::decorate(QWidget *mainWidget)
{
    return true;
}

void Product2Builder::onProduct2MenuActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString actionText = action->text();
        qDebug() << "Triggered action for Product 1: " << actionText;
        // You can now use the actionText as needed
    }
}
