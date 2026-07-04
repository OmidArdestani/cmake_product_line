#pragma once

#include "UniqueAsset4_global.h"

#include <pl_core/sharedassets.h>

class UNIQUEASSET4_EXPORT UniqueAsset4 : public IPLAsset
{
public:
    UniqueAsset4();

    // API Functions
private:
    QJsonObject runFeature(QJsonObject params);

public:
    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override {return enable;}
    APIFunctionMap getAPIFunctionMap() override;

private:
    bool enable{false};
};

