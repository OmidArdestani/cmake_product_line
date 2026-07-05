#include "WebSocketsAPI.h"
#include <QDebug>

#include "productlineapi.h"

WebSocketsAPI::WebSocketsAPI(ProductLineAPI *parent)
    : QObject{parent}
    , productLineAPI(parent)
    , server(new QWebSocketServer("WebSockets API Server", QWebSocketServer::NonSecureMode, this))
{
}

WebSocketsAPI::~WebSocketsAPI()
{
    stopServer();
}

// Starts the WebSocket server on the specified port
bool WebSocketsAPI::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port))
    {
        connect(server, &QWebSocketServer::newConnection, this, &WebSocketsAPI::onNewConnection, Qt::UniqueConnection);

        qDebug() << "WebSocket server started on port" << port;
        return true;
    }
    else
    {
        qWarning() << "Failed to start WebSocket server on port" << port;
        return false;
    }
}

// Stops the WebSocket server and closes all client connections
void WebSocketsAPI::stopServer()
{
    foreach (QWebSocket *client, clients)
    {
        client->close();
    }
    server->close();

    qDebug() << "WebSocket server stopped.";
}

// Handles new client connections
void WebSocketsAPI::onNewConnection()
{
    QWebSocket *client = server->nextPendingConnection();
    clients.append(client);

    connect(client, &QWebSocket::textMessageReceived, this, &WebSocketsAPI::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &WebSocketsAPI::onClientDisconnected);

    qDebug() << "New client connected:" << client->peerAddress().toString();
}

// Handles incoming text messages from clients
void WebSocketsAPI::onTextMessageReceived(const QString &message)
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client)
    {
        processRequest(client, message);
    }
}

// Handles client disconnections
void WebSocketsAPI::onClientDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client)
    {
        clients.removeAll(client);
        client->deleteLater();
        qDebug() << "Client disconnected.";
    }
}

// Processes incoming requests from clients
void WebSocketsAPI::processRequest(QWebSocket *client, const QString &request)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(request.toUtf8());
    if (!jsonDoc.isObject())
    {
        sendResponse(client, QJsonObject{{"result", QJsonValue()}, {"error", "Invalid JSON format"}});
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QString instanceId = jsonObj.value("instanceId").toString();
    QString method     = jsonObj.value("method").toString();
    QJsonObject params = jsonObj.value("params").toObject();

    if (method.isEmpty() || instanceId.isEmpty())
    {
        sendResponse(client, QJsonObject{{"result", QJsonValue()}, {"error", "Method name is missing"}});
        return;
    }

    // Handle different methods
    QJsonObject result = productLineAPI->callFunction(instanceId.toULongLong(), method, params);
    sendResponse(client, result);
}

// Sends a response to the client
void WebSocketsAPI::sendResponse(QWebSocket *client, const QJsonObject &response)
{
    QJsonDocument responseDoc(response);
    client->sendTextMessage(responseDoc.toJson(QJsonDocument::Compact));
}
