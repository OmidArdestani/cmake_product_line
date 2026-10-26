#pragma once

#include "UniqueAsset2_global.h"

#include <pl_core/sharedassets.h>
#include <memory>

namespace plapi {
class Asset2RunFeatureRequest;
class Asset2RunFeatureReply;
}

class UniqueAsset2Service;

class UNIQUEASSET2_EXPORT UniqueAsset2 : public IPLAsset
{
public:
    UniqueAsset2();
    ~UniqueAsset2() override;

    // Feature logic, called by UniqueAsset2Service (gRPC transport shim).
    void runFeature(const plapi::Asset2RunFeatureRequest& request,
                    plapi::Asset2RunFeatureReply& reply);

    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override {return enable;}
    QString assetName() const override{ return QStringLiteral("UniqueAsset2"); }
    grpc::Service* grpcService() override;

private:
    bool enable{false};
    std::unique_ptr<UniqueAsset2Service> service;
};

