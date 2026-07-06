#pragma once

#include <pl_core/sharedassets.h>

#include <QMap>
#include <QString>

#include <memory>
#include <thread>

namespace grpc {
class Server;
}

class CoreService;

// gRPC server for the product line.
//
// Register every asset of the active product with registerAsset() BEFORE
// calling start(). The server exposes:
//   - plapi.ProductLineCore          (asset discovery / enable control)
//   - one typed service per asset    (e.g. plapi.UniqueAsset1Service)
//
// THREADING: RPC handlers run on gRPC worker threads, NOT the Qt main
// thread. Handlers touching QWidget/UI state must marshal through
// QMetaObject::invokeMethod(target, ..., Qt::BlockingQueuedConnection).
class GrpcApiServer
{
public:
    explicit GrpcApiServer(quint16 port = 50051);
    ~GrpcApiServer();

    GrpcApiServer(const GrpcApiServer&) = delete;
    GrpcApiServer& operator=(const GrpcApiServer&) = delete;

    // Must be called before start(). Asset must outlive the server.
    void registerAsset(IPLAsset* asset);

    bool start();
    void stop();
    bool isRunning() const { return server != nullptr; }

    const QMap<QString, IPLAsset*>& assets() const { return assetMap; }

private:
    quint16 port;
    QMap<QString, IPLAsset*> assetMap;

    std::unique_ptr<CoreService> coreService;
    std::unique_ptr<grpc::Server> server;
    std::thread serverThread;
};
