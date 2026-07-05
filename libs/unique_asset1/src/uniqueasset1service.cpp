#include "uniqueasset1service.h"

#include "uniqueasset1.h"

UniqueAsset1Service::UniqueAsset1Service(UniqueAsset1* asset)
    : asset(asset)
{
}

grpc::Status UniqueAsset1Service::RunFeature(grpc::ServerContext* /*context*/,
                                             const plapi::Asset1RunFeatureRequest* request,
                                             plapi::Asset1RunFeatureReply* reply)
{
    if (!asset->isEnable())
    {
        return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                            "UniqueAsset1 is disabled");
    }

    asset->runFeature(*request, *reply);
    return grpc::Status::OK;
}
