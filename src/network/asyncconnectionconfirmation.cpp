#include "asyncconnectionconfirmation.h"

AsyncConnectionConfirmation::AsyncConnectionConfirmation(ServerConnectionsConfirmation::ConnectionService::AsyncService* service,
                                                         grpc::ServerCompletionQueue* cq)
    : m_service(service)
    , m_cq(cq)
    , m_responder(&m_context)
{
    // Запросить асинхронный приём вызова Connect (название метода из proto)
    m_service->RequestConnect(&m_context, &m_request, &m_responder, m_cq, m_cq, this);
}

void AsyncConnectionConfirmation::proceed(bool ok)
{
    if (!ok) {
        delete this;
        return;
    }

    switch (m_status) {
    case CREATE:
        m_status = PROCESS;

        new AsyncConnectionConfirmation(m_service, m_cq);

        emit proccess(api::server::ConnectionRequest::__name__, {});
        break;
    case PROCESS:
        break;
    case FINISH:
        delete this;
        break;
    default:
        delete this;
        break;
    }
}

void AsyncConnectionConfirmation::onFinished(const QVariantMap &data)
{
    ParameterHandler ph(data);

    bool result;
    if(!ph.handle(result, api::server::ConnectionResponse::Confirmation)){
        qWarning() << "Failed to handle confirmation parameter in AsyncConnectionConfirmation";
        result = false;
    }

    m_response.set_confirmation(result);
    m_status = FINISH;

    m_responder.Finish(m_response, grpc::Status::OK, this);
}
