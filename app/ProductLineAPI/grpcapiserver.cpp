#include "grpcapiserver.h"

#include "coreservice.h"

#include <grpcpp/grpcpp.h>

#include <QDebug>

namespace {
QString currentProductName()
{
#if defined(Product_1)
    return QStringLiteral("Product_1");
#elif defined(Product_2)
    return QStringLiteral("Product_2");
#elif defined(Product_3)
    return QStringLiteral("Product_3");
#elif defined(Product_4)
    return QStringLiteral("Product_4");
#elif defined(Product_5)
    return QStringLiteral("Product_5");
#else
    return QStringLiteral("Unknown");
#endif
}
} // namespace

GrpcApiServer::GrpcApiServer(quint16 port)
    : port(port)
{
}

GrpcApiServer::~GrpcApiServer()
{
    stop();
}

void GrpcApiServer::registerAsset(IPLAsset* asset)
{
    Q_ASSERT(asset);
    Q_ASSERT_X(!server, "GrpcApiServer::registerAsset",
               "assets must be registered before start()");
    assetMap.insert(asset->assetName(), asset);
}

bool GrpcApiServer::start()
{
    if (server)
        return true; // already running

    coreService = std::make_unique<CoreService>(assetMap, currentProductName());

    grpc::ServerBuilder builder;
    const std::string address = "0.0.0.0:" + std::to_string(port);
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());

    builder.RegisterService(coreService.get());
    for (auto* asset : assetMap)
        builder.RegisterService(asset->grpcService());

    server = builder.BuildAndStart();
    if (!server)
    {
        qWarning() << "GrpcApiServer: failed to start on" << QString::fromStdString(address);
        coreService.reset();
        return false;
    }

    // Wait() blocks until Shutdown(); run it on a dedicated thread so the
    // Qt event loop stays free. RPC dispatch uses gRPC's own thread pool.
    serverThread = std::thread([this] { server->Wait(); });

    qInfo() << "GrpcApiServer: listening on" << QString::fromStdString(address);
    return true;
}

void GrpcApiServer::stop()
{
    if (!server)
        return;

    server->Shutdown();
    if (serverThread.joinable())
        serverThread.join();

    server.reset();
    coreService.reset();

    qInfo() << "GrpcApiServer: stopped";
}
