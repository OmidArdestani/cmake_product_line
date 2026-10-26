#include "uniqueasset3.h"

#include "uniqueasset3service.h"

UniqueAsset3::UniqueAsset3()
    : service(std::make_unique<UniqueAsset3Service>(this))
{
}

UniqueAsset3::~UniqueAsset3() = default;

void UniqueAsset3::runFeature(const plapi::Asset3RunFeatureRequest& /*request*/,
                              plapi::Asset3RunFeatureReply& reply)
{
    reply.mutable_status()->set_code(0);
}

void UniqueAsset3::exceptionFunction()
{
    throw std::runtime_error("Error");
}

grpc::Service* UniqueAsset3::grpcService()
{
    return service.get();
}
