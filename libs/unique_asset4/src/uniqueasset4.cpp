#include "uniqueasset4.h"

#include "uniqueasset4service.h"

UniqueAsset4::UniqueAsset4()
    : service(std::make_unique<UniqueAsset4Service>(this))
{
}

UniqueAsset4::~UniqueAsset4() = default;

void UniqueAsset4::runFeature(const plapi::Asset4RunFeatureRequest& /*request*/,
                              plapi::Asset4RunFeatureReply& reply)
{
    reply.mutable_status()->set_code(0);
}

void UniqueAsset4::exceptionFunction()
{
    throw std::runtime_error("Error");
}

grpc::Service* UniqueAsset4::grpcService()
{
    return service.get();
}
