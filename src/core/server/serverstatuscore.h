#ifndef SERVERSTATUSCORE_H
#define SERVERSTATUSCORE_H

#include "src/interface/uinterface.h"

#include "api/external/server.h"

class ServerStatusCore : public UInterface
{
    Q_OBJECT
public:
    explicit ServerStatusCore(QObject *parent = nullptr);

    void registrationSubscribe() override;

private:
    void handleClientConnectionRequest(const QVariantMap& data);
};

#endif // SERVERSTATUSCORE_H
