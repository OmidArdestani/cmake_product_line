#include "IProductBuilder.h"

#pragma once


class Product4Builder : public IProductBuilder{
    Q_OBJECT
public:
    explicit Product4Builder();

    // IProductBuilder interface
public:
    bool buildMenu(QMenuBar *menuBar) override;
    bool registerFeatures(QMap<QString, IPLAsset *> &refMap) override;
    bool decorate(QWidget *mainWidget) override;

private slots:
    void onProduct4MenuActionTriggered();
};
