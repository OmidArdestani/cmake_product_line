#include "Product5Builder.h"

Product5Builder::Product5Builder()
    : IProductBuilder()
{

}

bool Product5Builder::buildMenu(QMenuBar *menuBar)
{
    QMenu *product5Menu = menuBar->addMenu("Product 5");
    product5Menu->addAction("Action 1 for Product 5", this, SLOT(onProduct5MenuActionTriggered()));
    product5Menu->addAction("Action 2 for Product 5", this, SLOT(onProduct5MenuActionTriggered()));
    return true;
}

bool Product5Builder::registerFeatures(QMap<QString, IPLAsset *> &refMap)
{
    return true;
}

bool Product5Builder::decorate(QWidget *mainWidget)
{
    return true;
}

void Product5Builder::onProduct5MenuActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString actionText = action->text();
        qDebug() << "Triggered action for Product 1: " << actionText;
        // You can now use the actionText as needed
    }
}
