#include "uniqueasset2.h"

UniqueAsset2::UniqueAsset2() {}


QJsonObject UniqueAsset2::runFeature(QJsonObject params)
{
    return QJsonObject();
}

void UniqueAsset2::exceptionFunction()
{
    throw std::runtime_error("Error");
}


APIFunctionMap UniqueAsset2::getAPIFunctionMap()
{
    APIFunctionMap apiFunctionMap;

    apiFunctionMap.insert("runFeature", BIND_API_FUNC(&UniqueAsset2::runFeature));


    return apiFunctionMap;
}
