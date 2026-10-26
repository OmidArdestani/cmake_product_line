#pragma once

#include "IProductBuilder.h"

class Product2Builder : public IProductBuilder
{
public:
    bool registerFeatures(QMap<QString, IPLAsset*>& assets) override;
};
