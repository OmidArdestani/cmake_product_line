#include "IProductBuilder.h"

#pragma once


class Product5Builder : public IProductBuilder{
    Q_OBJECT
public:
    explicit Product5Builder();

    // IProductBuilder interface
public:
    bool buildMenu(QMenuBar *menuBar) override;
    bool registerFeatures(QMap<QString, IPLAsset *> &refMap) override;
    bool decorate(QWidget *mainWidget) override;

private slots:
    void onProduct5MenuActionTriggered();
};
