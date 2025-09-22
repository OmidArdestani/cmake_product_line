#include "productlineapi.h"

#include "../mainwindow.h"

ProductLineAPI::ProductLineAPI(MainWindow* parent)
    : QObject{parent}
    , productLineMainWindow(parent)
    , grpcAPI(new GrpcAPI(this))
{
    // The main product line is defined as root
    insertInstance(0, productLineMainWindow);
}

QJsonObject ProductLineAPI::callFunction(quint64 instanceId, QString functionName, QJsonObject params)
{
    if(!instanceMap.contains(instanceId))
    {
        QJsonObject result{
            {"error","The instance is not found."}
        };

        return result;
    }

    auto instanceFunctionMap = instanceMap.value(instanceId)->getAPIFunctionMap();

    if(!instanceFunctionMap.contains(functionName))
    {
        QJsonObject result{
            {"error","function not found in the object"}
        };

        return result;
    }

    return (instanceFunctionMap.value(functionName))(params);
}

void ProductLineAPI::start()
{
    grpcAPI->startServer(1025);
}

void ProductLineAPI::stop()
{
    grpcAPI->stopServer();
}

void ProductLineAPI::insertInstance(const quint64 &key, IPLAsset *instance)
{
    instanceMap.insert(key, instance);
}

QJsonObject ProductLineAPI::getInstance(QJsonObject)
{

    QJsonObject result{
        { "instanceId", QString::number(reinterpret_cast<quintptr>(productLineMainWindow)) },
        { "error", "" }
    };

    return result;
}
