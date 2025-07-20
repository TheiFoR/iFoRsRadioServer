#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

#include "src/network/clienthandler.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer() override;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void newConnection(ClientHandler* clientHandler);
    void disconnected(ClientHandler* clientHandler);

private slots:
    void onClientDisconnected(ClientHandler* handler);

private:
    struct ThreadContext {
        QThread* thread;
        QList<ClientHandler*> handlers;
    };

    QVector<ThreadContext> m_threadPool;
    int m_maxThreads = 16;
    int m_nextThread = 0;
};

#endif // TCPSERVER_H
