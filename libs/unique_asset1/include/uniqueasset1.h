#pragma once

#include "UniqueAsset1_global.h"

#include <pl_core/sharedassets.h>
#include <memory>

namespace plapi {
class Asset1RunFeatureRequest;
class Asset1RunFeatureReply;
}

class UniqueAsset1Service;

class UNIQUEASSET1_EXPORT UniqueAsset1 : public IPLAsset
{
public:
    UniqueAsset1();
    ~UniqueAsset1() override;

    // Feature logic, called by UniqueAsset1Service (gRPC transport shim).
    void runFeature(const plapi::Asset1RunFeatureRequest& request,
                    plapi::Asset1RunFeatureReply& reply);

    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override{return enable;}
    QString assetName() const override{ return QStringLiteral("UniqueAsset1"); }
    grpc::Service* grpcService() override;

private:
    bool enable{false};
    std::unique_ptr<UniqueAsset1Service> service;
};

