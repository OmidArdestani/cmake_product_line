#pragma once

#include <pl_core.grpc.pb.h>

#include <QMap>
#include <QString>

class IPLAsset;

// Implements plapi.ProductLineCore: asset discovery and lifecycle control.
// Reads the asset map owned by GrpcApiServer (map is frozen before start()).
class CoreService final : public plapi::ProductLineCore::Service
{
public:
    CoreService(const QMap<QString, IPLAsset*>& assetMap, QString productName);

    grpc::Status ListAssets(grpc::ServerContext* context,
                            const plapi::ListAssetsRequest* request,
                            plapi::ListAssetsReply* reply) override;

    grpc::Status EnableAsset(grpc::ServerContext* context,
                             const plapi::EnableAssetRequest* request,
                             plapi::EnableAssetReply* reply) override;

    grpc::Status GetAssetState(grpc::ServerContext* context,
                               const plapi::GetAssetStateRequest* request,
                               plapi::GetAssetStateReply* reply) override;

private:
    const QMap<QString, IPLAsset*>& assetMap;
    QString productName;
};
