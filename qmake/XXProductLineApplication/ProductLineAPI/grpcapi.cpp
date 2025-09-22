#include "GrpcAPI.h"
#include "productlineapi.h"
#include <QDebug>
#include <QJsonDocument>

// ProductLineServiceImpl implementation
ProductLineServiceImpl::ProductLineServiceImpl(ProductLineAPI* api)
    : productLineAPI(api)
{
}

grpc::Status ProductLineServiceImpl::CallFunction(grpc::ServerContext* context,
                                                  const productline::CallFunctionRequest* request,
                                                  productline::CallFunctionResponse* response) {
    qDebug() << "gRPC CallFunction called - instanceId:" << request->instance_id() 
             << "function:" << QString::fromStdString(request->function_name());

    // Convert parameters from JSON string to QJsonObject
    QJsonDocument paramsDoc = QJsonDocument::fromJson(request->params_json().c_str());
    QJsonObject params = paramsDoc.object();

    // Call the function through ProductLineAPI
    QJsonObject result = productLineAPI->callFunction(request->instance_id(), 
                                                     QString::fromStdString(request->function_name()), 
                                                     params);

    // Convert result back to JSON string
    QJsonDocument resultDoc(result);
    response->set_result_json(resultDoc.toJson(QJsonDocument::Compact).toStdString());
    
    // Set error field if present
    if (result.contains("error") && !result["error"].toString().isEmpty()) {
        response->set_error(result["error"].toString().toStdString());
    } else {
        response->set_error("");
    }

    return grpc::Status::OK;
}

grpc::Status ProductLineServiceImpl::GetInstance(grpc::ServerContext* context,
                                                const productline::GetInstanceRequest* request,
                                                productline::GetInstanceResponse* response) {
    qDebug() << "gRPC GetInstance called";

    // Call getInstance through ProductLineAPI
    QJsonObject result = productLineAPI->getInstance(QJsonObject());
    
    if (result.contains("instanceId")) {
        bool ok;
        quint64 instanceId = result["instanceId"].toString().toULongLong(&ok);
        if (ok) {
            response->set_instance_id(instanceId);
        }
    }
    
    if (result.contains("error")) {
        response->set_error(result["error"].toString().toStdString());
    } else {
        response->set_error("");
    }

    return grpc::Status::OK;
}

grpc::Status ProductLineServiceImpl::InsertInstance(grpc::ServerContext* context,
                                                   const productline::InsertInstanceRequest* request,
                                                   productline::InsertInstanceResponse* response) {
    qDebug() << "gRPC InsertInstance called - instanceId:" << request->instance_id();

    // For now, we'll just return success since the original API doesn't expose insertInstance
    // This could be extended in the future if needed
    response->set_success(false);
    response->set_error("InsertInstance not supported via gRPC interface");

    return grpc::Status::OK;
}

// GrpcAPI implementation
GrpcAPI::GrpcAPI(ProductLineAPI *parent)
    : QObject{parent}
    , productLineAPI(parent)
    , serverThread(nullptr)
    , serverRunning(false)
{
    service = std::make_unique<ProductLineServiceImpl>(productLineAPI);
}

GrpcAPI::~GrpcAPI()
{
    stopServer();
}

bool GrpcAPI::startServer(quint16 port)
{
    if (serverRunning) {
        qWarning() << "gRPC server already running";
        return false;
    }

    grpc::ServerBuilder builder;
    
    // Listen on the given port without any authentication mechanism
    std::string server_address = "0.0.0.0:" + std::to_string(port);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // Register our service
    builder.RegisterService(service.get());
    
    // Assemble the server
    server = builder.BuildAndStart();
    
    if (server) {
        qDebug() << "gRPC server started on port" << port;
        serverRunning = true;
        
        // Create a thread to run the server
        serverThread = QThread::create([this]() {
            if (server) {
                server->Wait();
            }
        });
        serverThread->start();
        
        return true;
    } else {
        qWarning() << "Failed to start gRPC server on port" << port;
        return false;
    }
}

void GrpcAPI::stopServer()
{
    if (!serverRunning) {
        return;
    }

    if (server) {
        server->Shutdown();
        qDebug() << "gRPC server stopped.";
    }

    if (serverThread) {
        serverThread->quit();
        serverThread->wait();
        delete serverThread;
        serverThread = nullptr;
    }

    serverRunning = false;
}