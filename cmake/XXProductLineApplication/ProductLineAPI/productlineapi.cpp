#include "productlineapi.h"

ProductLineAPI::ProductLineAPI(IPLAsset* rootAsset, QObject* parent)
    : QObject{parent}
    , rootAsset(rootAsset)
    , webSocketsAPI(new WebSocketsAPI(this))
{
    // The root asset (instance 0) is the entry point for all API calls.
    insertInstance(0, rootAsset);
}

// Calls a function on a specific instance
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

// Starts the WebSocket server on the specified port
void ProductLineAPI::start(quint16 port)
{
    webSocketsAPI->startServer(port);
}

// Stops the WebSocket server
void ProductLineAPI::stop()
{
    webSocketsAPI->stopServer();
}

// Inserts an instance into the instance map
void ProductLineAPI::insertInstance(const quint64 &key, IPLAsset *instance)
{
    instanceMap.insert(key, instance);
}

// Returns the instance ID of the root asset so WebSocket clients can bootstrap
QJsonObject ProductLineAPI::getInstance(QJsonObject)
{
    QJsonObject result{
        { "instanceId", QString::number(reinterpret_cast<quintptr>(rootAsset)) },
        { "error", "" }
    };

    return result;
}
