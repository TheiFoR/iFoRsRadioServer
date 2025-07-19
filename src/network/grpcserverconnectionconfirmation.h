#ifndef GRPCSERVERCONNECTIONCONFIRMATION_H
#define GRPCSERVERCONNECTIONCONFIRMATION_H

#include "grpc/authorization/authorization.grpc.pb.h"
#include "src/interface/uinterface.h"

class GrpcServerConnectionConfirmation : public UInterface
{
    Q_OBJECT
public:
    explicit GrpcServerConnectionConfirmation(QObject *parent = nullptr);

    void registrationSubscribe() override;

    ServerConnectionsConfirmation::ConnectionService::AsyncService* asyncService();

private:
    ServerConnectionsConfirmation::ConnectionService::AsyncService m_asyncService;
};

#endif // GRPCSERVERCONNECTIONCONFIRMATION_H
