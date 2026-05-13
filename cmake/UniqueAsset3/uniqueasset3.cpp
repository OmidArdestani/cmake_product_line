#include "uniqueasset3.h"

UniqueAsset3::UniqueAsset3() {}

QJsonObject UniqueAsset3::runFeature(QJsonObject params)
{
    return QJsonObject();
}

void UniqueAsset3::exceptionFunction()
{
    throw std::runtime_error("Error");
}


APIFunctionMap UniqueAsset3::getAPIFunctionMap()
{
    APIFunctionMap apiFunctionMap;

    apiFunctionMap.insert("runFeature", BIND_API_FUNC(&UniqueAsset3::runFeature));


    return apiFunctionMap;
}
