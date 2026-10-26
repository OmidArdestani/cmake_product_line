#pragma once

#include "IProductBuilder.h"

class Product5Builder : public IProductBuilder
{
public:
    bool registerFeatures(QMap<QString, IPLAsset*>& assets) override;
};
