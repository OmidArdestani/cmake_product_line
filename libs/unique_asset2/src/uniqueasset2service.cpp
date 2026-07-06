#include "uniqueasset2service.h"

#include "uniqueasset2.h"

UniqueAsset2Service::UniqueAsset2Service(UniqueAsset2* asset)
    : asset(asset)
{
}

grpc::Status UniqueAsset2Service::RunFeature(grpc::ServerContext* /*context*/,
                                             const plapi::Asset2RunFeatureRequest* request,
                                             plapi::Asset2RunFeatureReply* reply)
{
    if (!asset->isEnable())
    {
        return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                            "UniqueAsset2 is disabled");
    }

    asset->runFeature(*request, *reply);
    return grpc::Status::OK;
}
