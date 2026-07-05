#pragma once


#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>

class ProductLineAPI;

class WebSocketsAPI : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketsAPI(ProductLineAPI *parent = nullptr);
    ~WebSocketsAPI();

    bool startServer(quint16 port);
    void stopServer();

signals:
    void methodCalled(const QString &methodName, const QJsonObject &params);

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onClientDisconnected();

private:
    ProductLineAPI* productLineAPI;

    QWebSocketServer* server;
    QList<QWebSocket*> clients;

    void processRequest(QWebSocket *client, const QString &request);
    void sendResponse(QWebSocket *client, const QJsonObject &response);
};


