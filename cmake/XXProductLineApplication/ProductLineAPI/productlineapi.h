#pragma once

#include "WebSocketsAPI.h"

#include <QObject>
#include <sharedassets.h>

class ProductLineAPI : public QObject
{
    Q_OBJECT
public:
    // rootAsset is registered as instance 0 and becomes the entry point for
    // all API calls. It must outlive this object.
    explicit ProductLineAPI(IPLAsset* rootAsset, QObject* parent = nullptr);

    QJsonObject callFunction(quint64 instanceId, QString functionName, QJsonObject params);

    // Starts the WebSocket server on the given port (default: 1025).
    void start(quint16 port = 1025);
    void stop();

    void insertInstance(const quint64& key, IPLAsset* instance);

    // Returns the instance ID of the root asset so clients can bootstrap
    // without needing out-of-band knowledge of the root pointer value.
    QJsonObject getInstance(QJsonObject params);

private:
    IPLAsset*      rootAsset     {nullptr};
    WebSocketsAPI* webSocketsAPI {nullptr};

    QMap<quint64, IPLAsset*> instanceMap;
};

