#pragma once

#include "WebSocketsAPI.h"

#include <QObject>
#include <pl_core/sharedassets.h>

class MainWindow;

class ProductLineAPI : public QObject
{
    Q_OBJECT
public:
    explicit ProductLineAPI(MainWindow* parent);

    QJsonObject callFunction(quint64 instanceId, QString functionName, QJsonObject params);

    void start();
    void stop();

    void insertInstance(const quint64& key, IPLAsset* instance);

private:
    QJsonObject getInstance(QJsonObject);

private:
    MainWindow* productLineMainWindow {nullptr};
    WebSocketsAPI* webSocketsAPI;

    QMap<quint64, IPLAsset*> instanceMap;
};

