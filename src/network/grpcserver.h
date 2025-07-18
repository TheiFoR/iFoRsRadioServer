#ifndef GRPCSERVER_H
#define GRPCSERVER_H

#include "src/interface/uinterface.h"
#include "src/utils/config.h"
#include "test.grpc.pb.h"
#include "src/utils/filemanager.h"
#include <grpcpp/grpcpp.h>

#include <QObject>

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class GrpcServer final : public UInterface, public HelloWorldTest::Greeter::Service {
    Q_OBJECT
public:
    explicit GrpcServer(QObject *parent = nullptr);
    ~GrpcServer();

    void registrationSubscribe() override;

    void start();
    void setAddress(const std::string& address);

    Status SayHello(ServerContext* context, const HelloWorldTest::HelloRequest* request,
                    HelloWorldTest::HelloReply* reply) override;
private:
    std::string m_address = "";
    bool m_isSecureChannel = false;

    FileManager m_fileManager;

    ServerBuilder m_builder;
};
#endif // GRPCSERVER_H
