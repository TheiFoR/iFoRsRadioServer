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

#include "src/core/connectionmanager.h"
#include "src/interface/uinterface.h"
#include "src/core/radio/radiostationscore.h"
#include "src/core/server/serverstatuscore.h"
#include "src/utils/parameterhandler.h"

class ClientHandler : public UInterface {
    Q_OBJECT

public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);
    ~ClientHandler();

    Q_INVOKABLE void registrationSubscribe() override;

public slots:
    void removalSuccessful() override;
    void start();

signals:
    void disconnected(ClientHandler* self);

private slots:
    void onDisconnected();

    void onReadyRead();
    void sendData(const QString& commandName, const QVariantMap& data);

private:
    bool m_registrationCompleted = false;

    QTcpSocket* m_socket = nullptr;
    qintptr m_descriptor;

    quint32 m_ip = 0;
    quint16 m_port = 0;
    QString m_address = "";

    QByteArray m_buffer;

    RadioStationsCore m_radioStationsCore{this};
    ServerStatusCore m_serverStatusCore{this};

    ConnectionManager m_connectionManager;

    void parseData();

    void handleConnectionRequest(const QVariantMap& data);
};

#endif // CLIENTHANDDLER_H
