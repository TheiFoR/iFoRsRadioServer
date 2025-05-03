#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMap>
#include <QDebug>

#include "clienthandler.h"
#include "src/modules/server/serverstatuscore.h"
#include "src/utils/config.h"

class Server : public UInterface
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void registrationSubscribe() override;

    void start();

private slots:
    void onNewConnection();
    void onClientDisconnected(ClientHandler *handler);

private:
    std::unique_ptr<QTcpServer> m_tcpServer;

    QMap<QThread*, ClientHandler*> m_clients;

    quint16 m_port = 1310;

    void loadSettings();
    void saveSettings();
};

#endif // SERVER_H
