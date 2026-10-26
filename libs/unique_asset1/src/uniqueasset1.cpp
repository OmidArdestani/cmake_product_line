#include "uniqueasset1.h"

#include "uniqueasset1service.h"

#include <QDateTime>

UniqueAsset1::UniqueAsset1()
    : service(std::make_unique<UniqueAsset1Service>(this))
{
}

UniqueAsset1::~UniqueAsset1() = default;

void UniqueAsset1::runFeature(const plapi::Asset1RunFeatureRequest& request,
                              plapi::Asset1RunFeatureReply& reply)
{
    reply.mutable_status()->set_code(0);
    reply.set_date_time(QDateTime::currentDateTime().toString(Qt::ISODate).toStdString());

    // Echo request params back to the caller.
    for (const auto& param : request.params())
    {
        *reply.add_loopback() = param;
    }
}

void UniqueAsset1::exceptionFunction()
{
    throw std::runtime_error("Error");
}

grpc::Service* UniqueAsset1::grpcService()
{
    return service.get();
}
