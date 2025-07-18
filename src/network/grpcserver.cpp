#include "grpcserver.h"

LOG_DECLARE(GRPCServer, Core)
LOG_DECLARE(GRPCServer, Status)
LOG_DECLARE(GRPCServer, Connection)
LOG_DECLARE(GRPCServer, Settings)

GrpcServer::GrpcServer(QObject *parent)
{

}

GrpcServer::~GrpcServer()
{

}

void GrpcServer::registrationSubscribe()
{

}

void GrpcServer::start()
{
    QString certificatePath = Config::getValue<QString>("Security", "certificatePath", "security/server.crt");
    QByteArray certificate = m_fileManager.read(certificatePath);
    if (!certificatePath.isEmpty() && !certificate.isEmpty()) {
        qCInfo(categoryGRPCServerStatus) << "Using certificate from path:" << certificatePath;
    }

    QString privateKeyPath = Config::getValue<QString>("Security", "privateKeyPath", "security/server.key");
    QByteArray privateKey = m_fileManager.read(privateKeyPath);
    if (!privateKeyPath.isEmpty() && !privateKey.isEmpty()) {
        qCInfo(categoryGRPCServerStatus) << "Using private key from path:" << privateKeyPath;
    }

    if((privateKeyPath.isEmpty() || privateKey.isEmpty()) && (certificatePath.isEmpty() || certificate.isEmpty())) {
        qCWarning(categoryGRPCServerStatus) << "Both certificate and private key are empty, using insecure channel";
    }
    else if ((privateKeyPath.isEmpty() || privateKey.isEmpty()) && !(certificatePath.isEmpty() || certificate.isEmpty())){
        qCWarning(categoryGRPCServerStatus) << "Private key is empty, using insecure channel";
    }
    else if (!(privateKeyPath.isEmpty() || privateKey.isEmpty()) && (certificatePath.isEmpty() || certificate.isEmpty())) {
        qCWarning(categoryGRPCServerStatus) << "Certificate is empty, using insecure channel";
    }
    else {
        qCInfo(categoryGRPCServerStatus) << "Both certificate and private key are provided, using secure channel";
        m_isSecureChannel = true;
    }

    std::shared_ptr<grpc::ServerCredentials> credentials;

    if(m_isSecureChannel){
        grpc::SslServerCredentialsOptions::PemKeyCertPair keyCertPair = {
            privateKey.toStdString(), certificate.toStdString()
        };

        grpc::SslServerCredentialsOptions sslOptions;
        sslOptions.pem_key_cert_pairs.push_back(keyCertPair);
        sslOptions.force_client_auth = false; // Set to true if you want to enforce client certificate validation

        credentials = grpc::SslServerCredentials(sslOptions);
    }
    else {
        credentials = grpc::InsecureServerCredentials();
    }

    m_builder.AddListeningPort(m_address, credentials);
    m_builder.RegisterService(this);

    std::unique_ptr<grpc::Server> server(m_builder.BuildAndStart());
    if (server) {
        qCInfo(categoryGRPCServerStatus) << "gRPC server started on address" << QString::fromStdString(m_address);
        server->Wait();
    } else {
        qCCritical(categoryGRPCServerStatus) << "Failed to start gRPC server on address" << QString::fromStdString(m_address);
    }
}

void GrpcServer::setAddress(const std::string &address)
{
    m_address = address;
}

Status GrpcServer::SayHello(ServerContext *context, const HelloWorldTest::HelloRequest *request, HelloWorldTest::HelloReply *reply) {
    std::string name = request->name();
    std::string response = "Hello, " + name;
    std::string client_address = context->peer();
    qInfo() << "Received request from: " << client_address;
    reply->set_message(response);
    qInfo() << "Handled SayHello for: " << name;
    return Status::OK;
}
