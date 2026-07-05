#include "productlineapi.h"

#include "mainwindow.h"

ProductLineAPI::ProductLineAPI(MainWindow* parent)
    : QObject{parent}
    , productLineMainWindow(parent)
    , webSocketsAPI(new WebSocketsAPI(this))
{
    // The main product line is defined as root
    insertInstance(0, productLineMainWindow);
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

// Starts the WebSocket server
void ProductLineAPI::start()
{
    webSocketsAPI->startServer(1025);
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

// Gets the main instance
QJsonObject ProductLineAPI::getInstance(QJsonObject)
{
    QJsonObject result{
        { "instanceId", QString::number(reinterpret_cast<quintptr>(productLineMainWindow)) },
        { "error", "" }
    };

    return result;
}
