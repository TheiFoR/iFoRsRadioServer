#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMap>
#include <QDebug>

#include "clienthandler.h"
#include "src/core/server/serverstatuscore.h"
#include "src/utils/config.h"
#include "src/types/clientcontext.h"
#include "src/network/tcpserver.h"

class Server : public UInterface
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void registrationSubscribe() override;

    void start();

private slots:
    void onNewConnection(ClientHandler* clientHandler);
    void onClientDisconnected(ClientHandler* clientHandler);

private:
    std::unique_ptr<TcpServer> m_tcpServer;

    // QMap<QTcpSocket*, ClientContext> m_clients;

    quint16 m_port = 1310;

    void loadSettings();
    void saveSettings();
};

#endif // SERVER_H
