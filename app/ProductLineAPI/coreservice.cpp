#include "coreservice.h"

#include <pl_core/sharedassets.h>

CoreService::CoreService(const QMap<QString, IPLAsset*>& assetMap, QString productName)
    : assetMap(assetMap)
    , productName(std::move(productName))
{
}

grpc::Status CoreService::ListAssets(grpc::ServerContext* /*context*/,
                                     const plapi::ListAssetsRequest* /*request*/,
                                     plapi::ListAssetsReply* reply)
{
    reply->mutable_status()->set_code(0);
    reply->set_product(productName.toStdString());

    for (auto it = assetMap.constBegin(); it != assetMap.constEnd(); ++it)
    {
        auto* info = reply->add_assets();
        info->set_name(it.key().toStdString());
        info->set_enabled(it.value()->isEnable());
    }

    return grpc::Status::OK;
}

grpc::Status CoreService::EnableAsset(grpc::ServerContext* /*context*/,
                                      const plapi::EnableAssetRequest* request,
                                      plapi::EnableAssetReply* reply)
{
    const QString name = QString::fromStdString(request->name());

    auto* asset = assetMap.value(name, nullptr);
    if (!asset)
    {
        return grpc::Status(grpc::StatusCode::NOT_FOUND,
                            "Unknown asset: " + request->name());
    }

    asset->enableAsset(request->enable());
    reply->mutable_status()->set_code(0);
    return grpc::Status::OK;
}

grpc::Status CoreService::GetAssetState(grpc::ServerContext* /*context*/,
                                        const plapi::GetAssetStateRequest* request,
                                        plapi::GetAssetStateReply* reply)
{
    const QString name = QString::fromStdString(request->name());

    auto* asset = assetMap.value(name, nullptr);
    if (!asset)
    {
        return grpc::Status(grpc::StatusCode::NOT_FOUND,
                            "Unknown asset: " + request->name());
    }

    reply->mutable_status()->set_code(0);
    auto* info = reply->mutable_asset();
    info->set_name(request->name());
    info->set_enabled(asset->isEnable());
    return grpc::Status::OK;
}
