#ifndef GRPCSERVER_H
#define GRPCSERVER_H

#include "src/interface/uinterface.h"
#include "src/network/grpceventloop.h"
#include "src/network/grpcserverconnectionconfirmation.h"
#include "src/utils/config.h"
#include "src/utils/filemanager.h"
#include <grpcpp/grpcpp.h>

#include <QObject>

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class GrpcServer final : public UInterface {
    Q_OBJECT
public:
    explicit GrpcServer(QObject *parent = nullptr);
    ~GrpcServer();

    void registrationSubscribe() override;

    void start();
    void setAddress(const std::string& address);

public slots:
    void onProccessed(const QString& commandName, const QVariantMap& data);

private:
    std::string m_address = "";
    bool m_isSecureChannel = false;

    std::shared_ptr<grpc::ServerCredentials> m_credentials = nullptr;
    std::unique_ptr<grpc::ServerCompletionQueue> m_cq = nullptr;
    std::unique_ptr<grpc::Server> m_server = nullptr;
    std::unique_ptr<GrpcEventLoop> m_eventLoop = nullptr;

    FileManager m_fileManager;
    ServerBuilder m_builder;

    GrpcServerConnectionConfirmation m_grpcConnectionConfirmation;

    std::shared_ptr<grpc::ServerCredentials> getCredential();
};
#endif // GRPCSERVER_H
