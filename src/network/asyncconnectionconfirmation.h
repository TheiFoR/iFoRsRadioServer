#ifndef ASYNCCONNECTIONCONFIRMATION_H
#define ASYNCCONNECTIONCONFIRMATION_H

#include "grpccallbase.h"
#include "grpc/authorization/authorization.grpc.pb.h"  // твой proto-сервис
#include <grpcpp/grpcpp.h>

#include "src/utils/parameterhandler.h"
#include "api/external/server.h"

class AsyncConnectionConfirmation : public GrpcCallBase
{
public:
    AsyncConnectionConfirmation(ServerConnectionsConfirmation::ConnectionService::AsyncService* service,
                                grpc::ServerCompletionQueue* cq);

    void proceed(bool ok) override;
    void onFinished(const QVariantMap& data) override;

private:
    ServerConnectionsConfirmation::ConnectionService::AsyncService* m_service;
    grpc::ServerCompletionQueue* m_cq;

    grpc::ServerContext m_context;
    ServerConnectionsConfirmation::ServerConnectionRequest m_request;
    ServerConnectionsConfirmation::ServerConnectionResponse m_response;
    grpc::ServerAsyncResponseWriter<ServerConnectionsConfirmation::ServerConnectionResponse> m_responder;

    CallStatus m_status = CREATE;
};

#endif // ASYNCCONNECTIONCONFIRMATION_H
