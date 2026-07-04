#pragma once

#include "IProductBuilder.h"

class Product2Builder : public IProductBuilder{
    Q_OBJECT
public:
    explicit Product2Builder();

    // IProductBuilder interface
public:
    bool buildMenu(QMenuBar *menuBar) override;
    bool registerFeatures(QMap<QString, IPLAsset *> &refMap) override;
    bool decorate(QWidget *mainWidget) override;

private slots:
    void onProduct2MenuActionTriggered();
};
