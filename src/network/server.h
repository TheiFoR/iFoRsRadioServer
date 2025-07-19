#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMap>
#include <QDebug>

#include "src/modules/server/serverstatuscore.h"
#include "src/network/grpcserver.h"
#include "src/utils/config.h"

namespace iFoRRadio{
class Server : public UInterface
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void registrationSubscribe() override;

    void start();

private:
    std::unique_ptr<GrpcServer> m_grpcServer;

    quint16 m_port = 1310;
    QString m_ip = "127.0.0.1";

    void loadSettings();
    void saveSettings();
};
}
#endif // SERVER_H
