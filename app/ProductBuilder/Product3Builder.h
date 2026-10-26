#pragma once

#include "IProductBuilder.h"

class Product3Builder : public IProductBuilder
{
public:
    bool registerFeatures(QMap<QString, IPLAsset*>& assets) override;
};
