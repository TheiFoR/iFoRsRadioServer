#include "server.h"

namespace iFoRRadio{

LOG_DECLARE(Server, Core);
LOG_DECLARE(Server, Status);
LOG_DECLARE(Server, Connection);

Server::Server(QObject *parent)
    : UInterface{parent}
{
    qCInfo(categoryServerCore) << "Create";

    loadSettings();
}

Server::~Server()
{
    saveSettings();
}

void Server::registrationSubscribe()
{
    qCInfo(categoryServerCore) << "Registration subscription started";

    qCInfo(categoryServerCore) << "Registration subscription completed";
}

void Server::start()
{
    m_grpcServer = std::make_unique<GrpcServer>();

    std::string address(m_ip.toStdString() + ":" + QString::number(m_port).toStdString());
    m_grpcServer->setAddress(address);

    m_grpcServer->start();
}

void Server::loadSettings()
{
    m_port = Config::getValue("Server", "port", m_port);
}

void Server::saveSettings()
{
    Config::setValue("Server", "port", m_port);
}

} // namespace iFoRRadio
