#include "grpcserver.h"
#include "src/network/asyncconnectionconfirmation.h"

LOG_DECLARE(GRPCServer, Core)
LOG_DECLARE(GRPCServer, Launch)
LOG_DECLARE(GRPCServer, Security)
LOG_DECLARE(GRPCServer, Connection)
LOG_DECLARE(GRPCServer, Settings)

GrpcServer::GrpcServer(QObject *parent)
{
    qCInfo(categoryGRPCServerCore) << "Created gRPC server";
}

GrpcServer::~GrpcServer()
{
    qCInfo(categoryGRPCServerCore) << "Destroyed gRPC server";
}

void GrpcServer::registrationSubscribe()
{
    qCInfo(categoryGRPCServerCore) << "Registration subscription started";



    qCInfo(categoryGRPCServerCore) << "Registration subscription completed";
}

std::shared_ptr<grpc::ServerCredentials> GrpcServer::getCredential()
{
    QString certificatePath = Config::getValue<QString>("Security", "certificatePath", "security/server.crt");
    QByteArray certificate = m_fileManager.read(certificatePath);
    if (!certificatePath.isEmpty() && !certificate.isEmpty()) {
        qCInfo(categoryGRPCServerSecurity) << "Using certificate from path:" << certificatePath;
    }

    QString privateKeyPath = Config::getValue<QString>("Security", "privateKeyPath", "security/server.key");
    QByteArray privateKey = m_fileManager.read(privateKeyPath);
    if (!privateKeyPath.isEmpty() && !privateKey.isEmpty()) {
        qCInfo(categoryGRPCServerSecurity) << "Using private key from path:" << privateKeyPath;
    }

    if((privateKeyPath.isEmpty() || privateKey.isEmpty()) && (certificatePath.isEmpty() || certificate.isEmpty())) {
        qCWarning(categoryGRPCServerSecurity) << "Both certificate and private key are empty, using insecure channel";
    }
    else if ((privateKeyPath.isEmpty() || privateKey.isEmpty()) && !(certificatePath.isEmpty() || certificate.isEmpty())){
        qCWarning(categoryGRPCServerSecurity) << "Private key is empty, using insecure channel";
    }
    else if (!(privateKeyPath.isEmpty() || privateKey.isEmpty()) && (certificatePath.isEmpty() || certificate.isEmpty())) {
        qCWarning(categoryGRPCServerSecurity) << "Certificate is empty, using insecure channel";
    }
    else {
        qCInfo(categoryGRPCServerSecurity) << "Both certificate and private key are provided, using secure channel";
        m_isSecureChannel = true;
    }

    if(m_isSecureChannel){
        grpc::SslServerCredentialsOptions::PemKeyCertPair keyCertPair = {
            privateKey.toStdString(), certificate.toStdString()
        };

        grpc::SslServerCredentialsOptions sslOptions;
        sslOptions.pem_key_cert_pairs.push_back(keyCertPair);
        sslOptions.force_client_auth = false; // Set to true if you want to enforce client certificate validation

        return grpc::SslServerCredentials(sslOptions);
    }
    else {
        return grpc::InsecureServerCredentials();
    }
}

void GrpcServer::start()
{
    qCInfo(categoryGRPCServerLaunch) << "Starting gRPC server";

    m_credentials = getCredential();

    if (!m_credentials) {
        qCCritical(categoryGRPCServerLaunch) << "Failed to create server credentials";
        return;
    }
    qCInfo(categoryGRPCServerLaunch) << "Server credentials created successfully";


    m_builder.AddListeningPort(m_address, m_credentials);
    qCDebug(categoryGRPCServerLaunch) << "gRPC server will listen on address" << QString::fromStdString(m_address);


    // Регистрируем AsyncService, который есть член класса GrpcServerConnectionConfirmation
    m_builder.RegisterService(m_grpcConnectionConfirmation.asyncService());
    qCDebug(categoryGRPCServerLaunch) << "gRPC service registered successfully";


    // Создаем CompletionQueue
    m_cq = m_builder.AddCompletionQueue();

    if (!m_cq) {
        qCCritical(categoryGRPCServerLaunch) << "Failed to create CompletionQueue";
        return;
    }
    qCDebug(categoryGRPCServerLaunch) << "CompletionQueue created successfully";


    // Строим и запускаем сервер
    m_server = m_builder.BuildAndStart();

    if (!m_server) {
        qCCritical(categoryGRPCServerLaunch) << "Failed to start gRPC server on address"
                                             << QString::fromStdString(m_address);
        return;
    }
    qCInfo(categoryGRPCServerLaunch) << "gRPC server started successfully on address"
                                     << QString::fromStdString(m_address);


    // Запускаем EventLoop в отдельном потоке
    m_eventLoop = std::make_unique<GrpcEventLoop>(m_cq.get(), this);

    if (!m_eventLoop) {
        qCCritical(categoryGRPCServerLaunch) << "Failed to create gRPC event loop";
        return;
    }
    qCDebug(categoryGRPCServerLaunch) << "gRPC event loop created successfully";


    m_eventLoop->start();
    qCDebug(categoryGRPCServerLaunch) << "gRPC event loop started successfully";


    new AsyncConnectionConfirmation(m_grpcConnectionConfirmation.asyncService(), m_cq.get());

    qCDebug(categoryGRPCServerLaunch) << "First AsyncConnectionConfirmation created, waiting for client connection";
}



void GrpcServer::setAddress(const std::string &address)
{
    m_address = address;
}

void GrpcServer::onProccessed(const QString &commandName, const QVariantMap &data)
{
    qCritical() << "GrpcServer::onProccessed called with commandName:" << commandName
                << "and data:" << data << " sender:" << sender();
}
