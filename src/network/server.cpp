#include "server.h"

LOG_DECLARE(Server, Core);
LOG_DECLARE(Server, Status);
LOG_DECLARE(Server, Connection);

Server::Server(QObject *parent)
    : UInterface{parent}
{
    qCInfo(categoryServerCore) << "Create";

    loadSettings();
}

Server::~Server()
{
    saveSettings();
}

void Server::registrationSubscribe()
{
    qCInfo(categoryServerCore) << "Registration subscription started";

    qCInfo(categoryServerCore) << "Registration subscription completed";
}

void Server::start()
{
    m_tcpServer = std::make_unique<QTcpServer>();

    if (!m_tcpServer->listen(QHostAddress::Any, m_port)) {
        qCCritical(categoryServerStatus) << "Error starting the server:" << m_tcpServer->errorString();
        return;
    }

    connect(m_tcpServer.get(), &QTcpServer::newConnection, this, &Server::onNewConnection);

    qCInfo(categoryServerStatus) << "Server started on port" << m_port;
}

void Server::onNewConnection()
{
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    if (!clientSocket) {
        qCWarning(categoryServerConnection) << "Failed to accept new connection";
        return;
    }

    QThread *clientThread = new QThread(this);
    ClientHandler *clientHandler = new ClientHandler(clientSocket, this);

    registrateTransfer(clientHandler, this);


    connect(clientThread, &QThread::started, clientHandler, &ClientHandler::start);

    connect(clientThread, &QThread::finished, clientHandler, &ClientHandler::deleteLater);
    connect(clientThread, &QThread::finished, clientThread, &QThread::deleteLater);

    connect(clientHandler, &ClientHandler::disconnected, this, &Server::onClientDisconnected);

    m_clients.insert(clientThread, clientHandler);

    clientHandler->moveToThread(clientThread);
    clientHandler->start();

    qCInfo(categoryServerConnection) << "New connection from:" << clientSocket->peerAddress().toString();
}

void Server::onClientDisconnected(ClientHandler *handler)
{
    QThread *clientThread = m_clients.key(handler);
    if (clientThread) {
        qCInfo(categoryServerConnection) << "Client disconnected, removing handler from server";
        m_clients.remove(clientThread);
        clientThread->quit();
    } else {
        qCWarning(categoryServerConnection) << "Could not find client thread for handler";
    }
}

void Server::loadSettings()
{
    m_port = Config::getValue("Server", "port", m_port);
}

void Server::saveSettings()
{
    Config::setValue("Server", "port", m_port);
}
