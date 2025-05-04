#ifndef CLIENTHANDDLER_H
#define CLIENTHANDDLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMap>
#include <QDebug>
#include <QLoggingCategory>

#include "api/external/server.h"

#include "src/interface/uinterface.h"
#include "src/modules/radio/radiostationscore.h"
#include "src/modules/server/serverstatuscore.h"
#include "src/utils/parameterhandler.h"

class ClientHandler : public UInterface {
    Q_OBJECT

public:
    explicit ClientHandler(QTcpSocket *socket, QObject *parent = nullptr);
    ~ClientHandler();

    void registrationSubscribe() override;

public slots:
    void start();

signals:
    void disconnected(ClientHandler *handler);

private slots:
    void onReadyRead();
    void onDisconnected();

    void sendData(const QString& commandName, const QVariantMap& data);

private:
    bool m_registrationComplete = false;
    QTcpSocket *m_socket;
    std::list<std::pair<const QString&, const QVariantMap&>> m_lostPackets;

    QByteArray m_buffer;
    quint64 m_expectedSize = 0;
    const qsizetype m_datasizePacketSize = 29;

    RadioStationsCore m_radioStationsCore;
    ServerStatusCore m_serverStatusCore;

    void handleConnectionRequest(const QVariantMap& data);

    void sendLostPacket();
};

#endif // CLIENTHANDDLER_H
