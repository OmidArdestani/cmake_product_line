#include "uniqueasset3service.h"

#include "uniqueasset3.h"

UniqueAsset3Service::UniqueAsset3Service(UniqueAsset3* asset)
    : asset(asset)
{
}

grpc::Status UniqueAsset3Service::RunFeature(grpc::ServerContext* /*context*/,
                                             const plapi::Asset3RunFeatureRequest* request,
                                             plapi::Asset3RunFeatureReply* reply)
{
    if (!asset->isEnable())
    {
        return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                            "UniqueAsset3 is disabled");
    }

    asset->runFeature(*request, *reply);
    return grpc::Status::OK;
}
