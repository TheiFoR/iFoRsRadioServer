#include "grpcserverconnectionconfirmation.h"

LOG_DECLARE(GRPCServer, ConnectionConfirmation)

GrpcServerConnectionConfirmation::GrpcServerConnectionConfirmation(QObject *parent)
    : UInterface(parent)
{
    // Инициализация если нужна
}

void GrpcServerConnectionConfirmation::registrationSubscribe()
{
    // Тут можно создать первый AsyncCall, например
}

ServerConnectionsConfirmation::ConnectionService::AsyncService* GrpcServerConnectionConfirmation::asyncService()
{
    return &m_asyncService;
}
