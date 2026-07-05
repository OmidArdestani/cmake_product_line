#pragma once

#include <unique_asset2.grpc.pb.h>

class UniqueAsset2;

// gRPC service implementation for UniqueAsset2.
// Thin transport shim: converts proto messages and delegates to the asset.
class UniqueAsset2Service final : public plapi::UniqueAsset2Service::Service
{
public:
    explicit UniqueAsset2Service(UniqueAsset2* asset);

    grpc::Status RunFeature(grpc::ServerContext* context,
                            const plapi::Asset2RunFeatureRequest* request,
                            plapi::Asset2RunFeatureReply* reply) override;

private:
    UniqueAsset2* asset;
};
