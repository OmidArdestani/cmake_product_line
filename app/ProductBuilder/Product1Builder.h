#pragma once

#include "IProductBuilder.h"

class Product1Builder : public IProductBuilder
{
public:
    bool registerFeatures(QMap<QString, IPLAsset*>& assets) override;
};
