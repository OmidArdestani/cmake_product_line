#pragma once

#include <QObject>
#include <functional>
#include <QJsonObject>


#define BIND_API_FUNC(function) std::bind(function, this, std::placeholders::_1)

using APIFunctionMap = QMap<QString, std::function<QJsonObject (QJsonObject)>>;

class IPLAsset{
public:
    virtual void enableAsset(bool value) = 0;
    virtual bool isEnable() const = 0;

    virtual APIFunctionMap getAPIFunctionMap() = 0;
};


class SharedAssets
{
public:
    SharedAssets();
};

