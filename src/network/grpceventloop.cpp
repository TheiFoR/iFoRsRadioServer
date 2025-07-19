#include "grpceventloop.h"
#include "grpcserver.h"

GrpcEventLoop::GrpcEventLoop(grpc::ServerCompletionQueue *cq, QObject *parent)
    : QThread(parent), m_cq(cq) {}

void GrpcEventLoop::run() {
    void* tag;
    bool ok;
    while (m_cq->Next(&tag, &ok)) {
        auto* call = static_cast<GrpcCallBase*>(tag);

        GrpcServer* server = static_cast<GrpcServer*>(parent());

        GrpcServer::connect(call, &GrpcCallBase::proccess, server, &GrpcServer::onProccessed, Qt::QueuedConnection);

        call->proceed(ok);
    }
}
