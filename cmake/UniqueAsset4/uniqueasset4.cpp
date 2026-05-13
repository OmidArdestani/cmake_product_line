#include "uniqueasset4.h"

UniqueAsset4::UniqueAsset4() {}

QJsonObject UniqueAsset4::runFeature(QJsonObject params)
{
    return QJsonObject();
}

void UniqueAsset4::exceptionFunction()
{
    throw std::runtime_error("Error");
}


APIFunctionMap UniqueAsset4::getAPIFunctionMap()
{
    APIFunctionMap apiFunctionMap;

    apiFunctionMap.insert("runFeature", BIND_API_FUNC(&UniqueAsset4::runFeature));


    return apiFunctionMap;
}
