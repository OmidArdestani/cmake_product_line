#include "uniqueasset4service.h"

#include "uniqueasset4.h"

UniqueAsset4Service::UniqueAsset4Service(UniqueAsset4* asset)
    : asset(asset)
{
}

grpc::Status UniqueAsset4Service::RunFeature(grpc::ServerContext* /*context*/,
                                             const plapi::Asset4RunFeatureRequest* request,
                                             plapi::Asset4RunFeatureReply* reply)
{
    if (!asset->isEnable())
    {
        return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                            "UniqueAsset4 is disabled");
    }

    asset->runFeature(*request, *reply);
    return grpc::Status::OK;
}
