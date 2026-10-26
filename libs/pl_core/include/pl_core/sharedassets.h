#pragma once

#include <QString>

namespace grpc {
class Service;
}

// Interface every unique asset implements.
//
// Each asset owns a typed gRPC service (defined in proto/unique_assetN.proto,
// generated into pl_proto). The application registers asset services on its
// gRPC server at startup.
//
// THREADING: gRPC handlers run on gRPC worker threads, NOT the Qt main
// thread. Handlers touching QWidget/UI state must marshal through
// QMetaObject::invokeMethod(target, ..., Qt::BlockingQueuedConnection).
class IPLAsset{
public:
    virtual ~IPLAsset() = default;

    virtual void enableAsset(bool value) = 0;
    virtual bool isEnable() const = 0;

    // Stable identifier used by the core service (e.g. "UniqueAsset1").
    virtual QString assetName() const = 0;

    // The asset's gRPC service implementation. Owned by the asset;
    // must outlive the running server.
    virtual grpc::Service* grpcService() = 0;
};


class SharedAssets
{
public:
    SharedAssets();
};

