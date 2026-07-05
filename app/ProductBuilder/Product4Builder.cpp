#include "Product4Builder.h"

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
