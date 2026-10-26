#pragma once

#include "UniqueAsset4_global.h"

#include <pl_core/sharedassets.h>
#include <memory>

namespace plapi {
class Asset4RunFeatureRequest;
class Asset4RunFeatureReply;
}

class UniqueAsset4Service;

class UNIQUEASSET4_EXPORT UniqueAsset4 : public IPLAsset
{
public:
    UniqueAsset4();
    ~UniqueAsset4() override;

    // Feature logic, called by UniqueAsset4Service (gRPC transport shim).
    void runFeature(const plapi::Asset4RunFeatureRequest& request,
                    plapi::Asset4RunFeatureReply& reply);

    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override {return enable;}
    QString assetName() const override{ return QStringLiteral("UniqueAsset4"); }
    grpc::Service* grpcService() override;

private:
    bool enable{false};
    std::unique_ptr<UniqueAsset4Service> service;
};

