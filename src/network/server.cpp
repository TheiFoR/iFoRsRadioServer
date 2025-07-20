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
    m_tcpServer = std::make_unique<TcpServer>();

    if (!m_tcpServer->listen(QHostAddress::Any, m_port)) {
        qCCritical(categoryServerStatus) << "Error starting the server:" << m_tcpServer->errorString();
        return;
    }

    connect(m_tcpServer.get(), &TcpServer::newConnection, this, &Server::onNewConnection);
    connect(m_tcpServer.get(), &TcpServer::disconnected, this, &Server::onClientDisconnected);

    qCInfo(categoryServerStatus) << "Server started on port" << m_port;
}

void Server::onNewConnection(ClientHandler* clientHandler)
{
    qCInfo(categoryServerConnection) << "New connection" << clientHandler->strId();
}

void Server::onClientDisconnected(ClientHandler* clientHandler)
{
    qCInfo(categoryServerConnection) << "Client success disconnected!" << clientHandler->strId();
}

void Server::loadSettings()
{
    m_port = Config::getValue("Server", "port", m_port);
}

void Server::saveSettings()
{
    Config::setValue("Server", "port", m_port);
}
