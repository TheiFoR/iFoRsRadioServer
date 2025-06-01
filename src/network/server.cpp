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

    std::shared_ptr<QThread> clientThread = std::make_shared<QThread>(this);
    std::shared_ptr<ClientHandler> clientHandler = std::make_shared<ClientHandler>(clientSocket);

    registrateTransfer(clientHandler.get(), this);

    connect(clientThread.get(), &QThread::started, clientHandler.get(), &ClientHandler::start);

    connect(clientHandler.get(), &ClientHandler::disconnected, this, &Server::onClientDisconnected);

    m_clients.insert(clientSocket, ClientContext{clientHandler, clientThread});

    clientHandler->moveToThread(clientThread.get());
    clientThread->start();

    qCInfo(categoryServerConnection) << "New connection from:" << clientSocket->peerAddress().toString();
}

void Server::onClientDisconnected(QTcpSocket *socket)
{
    ClientContext clientContext = m_clients.value(socket);
    if (clientContext.thread.get()) {
        m_clients.remove(socket);
        qCInfo(categoryServerConnection) << "Client disconnected, removing handler from server:" << "ClientContext[ Handler:" << clientContext.handler.get()
                                         << "Thread:" << clientContext.thread.get() << "Count: [" << clientContext.handler.use_count() << clientContext.thread.use_count() << "] ]";
        clientContext.thread->quit();
        clientContext.thread->wait();
    } else {
        qCWarning(categoryServerConnection) << "Could not find client thread for handler";
    }
    qCInfo(categoryServerConnection) << "Client success disconnected!";
}

void Server::loadSettings()
{
    m_port = Config::getValue("Server", "port", m_port);
}

void Server::saveSettings()
{
    Config::setValue("Server", "port", m_port);
}
