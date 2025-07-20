#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent)
    : QTcpServer{parent}
{
    for (int i = 0; i < m_maxThreads; ++i) {
        QThread* thread = new QThread(this);
        thread->setObjectName(QString("ClientThread_%1").arg(i));
        thread->start();

        ThreadContext threadContext;
        threadContext.thread = thread;
        threadContext.handlers = QList<ClientHandler*>();
        m_threadPool.append(threadContext);
    }
}

TcpServer::~TcpServer()
{
    for (auto& ctx : m_threadPool) {
        ctx.thread->quit();
        ctx.thread->wait();
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    ThreadContext &ctx = m_threadPool[m_nextThread];
    m_nextThread = (m_nextThread + 1) % m_maxThreads;

    ClientHandler *handler = new ClientHandler(socketDescriptor);
    handler->moveToThread(ctx.thread);
    ctx.handlers.append(handler);

    connect(handler, &ClientHandler::disconnected, this, &TcpServer::onClientDisconnected);

    QMetaObject::invokeMethod(handler, "registrationSubscribe", Qt::QueuedConnection);
    QMetaObject::invokeMethod(handler, "start", Qt::QueuedConnection);

    emit newConnection(handler);
}

void TcpServer::onClientDisconnected(ClientHandler *handler) {
    for (auto& ctx : m_threadPool) {
        if (ctx.handlers.contains(handler)) {
            ctx.handlers.removeAll(handler);
            handler->deleteLater();

            emit disconnect(handler);
            return;
        }
    }
}
