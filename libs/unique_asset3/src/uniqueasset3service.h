#pragma once

#include <unique_asset3.grpc.pb.h>

class UniqueAsset3;

// gRPC service implementation for UniqueAsset3.
// Thin transport shim: converts proto messages and delegates to the asset.
class UniqueAsset3Service final : public plapi::UniqueAsset3Service::Service
{
public:
    explicit UniqueAsset3Service(UniqueAsset3* asset);

    grpc::Status RunFeature(grpc::ServerContext* context,
                            const plapi::Asset3RunFeatureRequest* request,
                            plapi::Asset3RunFeatureReply* reply) override;

private:
    UniqueAsset3* asset;
};
