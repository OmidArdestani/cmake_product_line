#include "uniqueasset2.h"

#include "uniqueasset2service.h"

UniqueAsset2::UniqueAsset2()
    : service(std::make_unique<UniqueAsset2Service>(this))
{
}

UniqueAsset2::~UniqueAsset2() = default;

void UniqueAsset2::runFeature(const plapi::Asset2RunFeatureRequest& /*request*/,
                              plapi::Asset2RunFeatureReply& reply)
{
    reply.mutable_status()->set_code(0);
}

void UniqueAsset2::exceptionFunction()
{
    throw std::runtime_error("Error");
}

grpc::Service* UniqueAsset2::grpcService()
{
    return service.get();
}
