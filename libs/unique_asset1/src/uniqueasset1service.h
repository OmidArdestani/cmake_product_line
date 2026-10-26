#pragma once

#include <unique_asset1.grpc.pb.h>

class UniqueAsset1;

// gRPC service implementation for UniqueAsset1.
// Thin transport shim: converts proto messages and delegates to the asset.
class UniqueAsset1Service final : public plapi::UniqueAsset1Service::Service
{
public:
    explicit UniqueAsset1Service(UniqueAsset1* asset);

    grpc::Status RunFeature(grpc::ServerContext* context,
                            const plapi::Asset1RunFeatureRequest* request,
                            plapi::Asset1RunFeatureReply* reply) override;

private:
    UniqueAsset1* asset;
};
