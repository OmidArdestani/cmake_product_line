#pragma once

#include <QMap>
#include <pl_core/sharedassets.h>

// Product composition only. Menus and tool handlers belong to the application.
class IProductBuilder
{
public:
    virtual ~IProductBuilder() = default;
    virtual bool registerFeatures(QMap<QString, IPLAsset*>& assets) = 0;
};
