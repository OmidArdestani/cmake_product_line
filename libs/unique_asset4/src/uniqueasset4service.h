#pragma once

#include <unique_asset4.grpc.pb.h>

class UniqueAsset4;

// gRPC service implementation for UniqueAsset4.
// Thin transport shim: converts proto messages and delegates to the asset.
class UniqueAsset4Service final : public plapi::UniqueAsset4Service::Service
{
public:
    explicit UniqueAsset4Service(UniqueAsset4* asset);

    grpc::Status RunFeature(grpc::ServerContext* context,
                            const plapi::Asset4RunFeatureRequest* request,
                            plapi::Asset4RunFeatureReply* reply) override;

private:
    UniqueAsset4* asset;
};
