#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QThread>

#include "src/core/connectionmanager.h"
#include "src/network/server.h"
#include "src/utils/config.h"
#include "src/interface/uinterface.h"

class Core : public UInterface
{
    Q_OBJECT

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void registrationSubscribe() override;

private:
    ConnectionManager m_connectionManager;

    QThread m_serverThread;
    Server m_server;
};

#endif // CORE_H
