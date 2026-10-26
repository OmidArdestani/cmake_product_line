#include "Product4Builder.h"

#include <uniqueasset2.h>
#include <uniqueasset4.h>

Product4Builder::Product4Builder()
    : IProductBuilder()
{

}

bool Product4Builder::buildMenu(QMenuBar *menuBar)
{
    QMenu *product4Menu = menuBar->addMenu("Product 4");
    product4Menu->addAction("Action 1 for Product 4", this, SLOT(onProduct4MenuActionTriggered()));
    product4Menu->addAction("Action 2 for Product 4", this, SLOT(onProduct4MenuActionTriggered()));
    return true;
}

bool Product4Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    // Product_4 ships UniqueAsset2 + UniqueAsset4.
    auto* asset2 = new UniqueAsset2();
    auto* asset4 = new UniqueAsset4();
    refMap.insert(asset2->assetName(), asset2);
    refMap.insert(asset4->assetName(), asset4);
    return true;
}

bool Product4Builder::decorate(QWidget *mainWidget)
{
    return true;
}

void Product4Builder::onProduct4MenuActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString actionText = action->text();
        qDebug() << "Triggered action for Product 1: " << actionText;
        // You can now use the actionText as needed
    }
}
