#pragma once

#include "UniqueAsset3_global.h"

#include <pl_core/sharedassets.h>
#include <memory>

namespace plapi {
class Asset3RunFeatureRequest;
class Asset3RunFeatureReply;
}

class UniqueAsset3Service;

class UNIQUEASSET3_EXPORT UniqueAsset3 : public IPLAsset
{
public:
    UniqueAsset3();
    ~UniqueAsset3() override;

    // Feature logic, called by UniqueAsset3Service (gRPC transport shim).
    void runFeature(const plapi::Asset3RunFeatureRequest& request,
                    plapi::Asset3RunFeatureReply& reply);

    void exceptionFunction();

    // IPLAsset interface
public:
    void enableAsset(bool value) override{ enable = value;}
    bool isEnable() const override {return enable;}
    QString assetName() const override{ return QStringLiteral("UniqueAsset3"); }
    grpc::Service* grpcService() override;

private:
    bool enable{false};
    std::unique_ptr<UniqueAsset3Service> service;
};

