#pragma once

#include "UniqueAsset1_global.h"

#include <pl_core/sharedassets.h>

class UNIQUEASSET1_EXPORT UniqueAsset1 : public IPLAsset
{
public:
    UniqueAsset1();

    // API Functions
private:
    QJsonObject runFeature(QJsonObject params);

public:
    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override{return enable;}
    APIFunctionMap getAPIFunctionMap() override;

private:
    bool enable{false};
};

