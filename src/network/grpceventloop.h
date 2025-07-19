#ifndef GRPCEVENTLOOP_H
#define GRPCEVENTLOOP_H

#include <QThread>

#include <grpcpp/grpcpp.h>

#include "src/network/grpccallbase.h"

class GrpcEventLoop : public QThread
{
    Q_OBJECT
public:
    explicit GrpcEventLoop(grpc::ServerCompletionQueue* cq, QObject* parent = nullptr);

    void run() override;

private:
    grpc::ServerCompletionQueue* m_cq;
};

#endif // GRPCEVENTLOOP_H
