#pragma once

#include "GrpcAPI.h"

#include <QObject>
#include <sharedassets.h>

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
    QJsonObject getInstance(QJsonObject);

private:

private:
    MainWindow* productLineMainWindow {nullptr};
    GrpcAPI* grpcAPI;

    QMap<quint64, IPLAsset*> instanceMap;
};

