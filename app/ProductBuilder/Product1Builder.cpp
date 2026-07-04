#include "Product1Builder.h"

Product1Builder::Product1Builder()
    : IProductBuilder()
{
}

bool Product1Builder::buildMenu(QMenuBar *menuBar)
{
    QMenu *product1Menu = menuBar->addMenu("Product 1");
    product1Menu->addAction("Action 1 for Product 1", this, SLOT(onProduct1MenuActionTriggered()));
    product1Menu->addAction("Action 2 for Product 1", this, SLOT(onProduct1MenuActionTriggered()));

    return true;
}

bool Product1Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    return true;
}

bool Product1Builder::decorate(QWidget *mainWidget)
{
    return true;
}

void Product1Builder::onProduct1MenuActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString actionText = action->text();
        qDebug() << "Triggered action for Product 1: " << actionText;
        // You can now use the actionText as needed
    }
}
