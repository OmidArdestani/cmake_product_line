#pragma once

#include <grpcpp/grpcpp.h>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include "productline_api.grpc.pb.h"

class ProductLineAPI;

// gRPC service implementation
class ProductLineServiceImpl final : public productline::ProductLineService::Service {
public:
    explicit ProductLineServiceImpl(ProductLineAPI* api);

    grpc::Status CallFunction(grpc::ServerContext* context,
                             const productline::CallFunctionRequest* request,
                             productline::CallFunctionResponse* response) override;

    grpc::Status GetInstance(grpc::ServerContext* context,
                            const productline::GetInstanceRequest* request,
                            productline::GetInstanceResponse* response) override;

    grpc::Status InsertInstance(grpc::ServerContext* context,
                               const productline::InsertInstanceRequest* request,
                               productline::InsertInstanceResponse* response) override;

private:
    ProductLineAPI* productLineAPI;
};

// gRPC server wrapper class
class GrpcAPI : public QObject
{
    Q_OBJECT

public:
    explicit GrpcAPI(ProductLineAPI *parent = nullptr);
    ~GrpcAPI();

    bool startServer(quint16 port);
    void stopServer();

signals:
    void methodCalled(const QString &methodName, const QJsonObject &params);

private:
    ProductLineAPI* productLineAPI;
    std::unique_ptr<grpc::Server> server;
    std::unique_ptr<ProductLineServiceImpl> service;
    QThread* serverThread;
    bool serverRunning;
    
    void runServerInThread();
};