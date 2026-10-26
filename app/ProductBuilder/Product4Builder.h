#pragma once

#include "IProductBuilder.h"

class Product4Builder : public IProductBuilder
{
public:
    bool registerFeatures(QMap<QString, IPLAsset*>& assets) override;
};
