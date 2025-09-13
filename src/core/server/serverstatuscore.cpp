#include "serverstatuscore.h"

LOG_DECLARE(ServerStatusCore, Core)
LOG_DECLARE(ServerStatusCore, Connection)

ServerStatusCore::ServerStatusCore(QObject *parent)
    : UInterface{parent}
{
    qCInfo(categoryServerStatusCoreCore) << "Create";
    setUseId(true);
}

void ServerStatusCore::registrationSubscribe()
{
    qCInfo(categoryServerStatusCoreCore) << "Registration subscription started";

    emit createSubscribe(api::server::ServerConnectionResponse::__name__, this);
    emit subscribe(api::server::ServerConnectionRequest::__name__, this, std::bind(&ServerStatusCore::handleClientConnectionRequest, this, std::placeholders::_1));

    qCInfo(categoryServerStatusCoreCore) << "Registration subscription completed";
}

void ServerStatusCore::handleClientConnectionRequest(const QVariantMap &data)
{
    qCInfo(categoryServerStatusCoreConnection) << "New connection request";

    QVariantMap result;
    result[api::server::ServerConnectionResponse::Confirmation] = true;

    emit signalUPacket(api::server::ServerConnectionResponse::__name__, result);

    qCInfo(categoryServerStatusCoreConnection) << "Approved";
}
