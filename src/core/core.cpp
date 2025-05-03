#include "core.h"

LOG_DECLARE(Core, Base);

Core::Core(QObject *parent)
    : UInterface(parent)
{
    qCInfo(categoryCoreBase) << "Create";

    connect(&m_serverThread, &QThread::started, &m_server, &Server::start);
    connect(&m_serverThread, &QThread::finished, &m_server, &QObject::deleteLater);
}

Core::~Core() {
    qCInfo(categoryCoreBase) << "Shutting down Core...";

    qCDebug(categoryCoreBase) << "Stopping server thread...";

    m_serverThread.quit();
    if (!m_serverThread.wait(5000)) {
        qCWarning(categoryCoreBase) << "Server thread did not quit in time, forcing termination!";
        m_serverThread.terminate();
    } else {
        qCInfo(categoryCoreBase) << "Server thread exited cleanly.";
    }

    qCInfo(categoryCoreBase) << "Core destroyed.";
}


void Core::registrationSubscribe()
{
    qCInfo(categoryCoreBase) << "Registration subscription started";

    connect(this, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::subscribe), &m_connectionManager, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&ConnectionManager::handleSubscriber));
    connect(this, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::subscribe), &m_connectionManager, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&ConnectionManager::handleSubscriber));

    connect(this, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::unsubscribe), &m_connectionManager, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&ConnectionManager::handleSubscriber));
    connect(this, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::unsubscribe), &m_connectionManager, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&ConnectionManager::handleSubscriber));

    connect(this, &UInterface::createSubscribe, &m_connectionManager, &ConnectionManager::handleCreateSubscribe);
    connect(this, &UInterface::removeSubscribe, &m_connectionManager, &ConnectionManager::handleRemoveSubscribe);

    registrateTransfer(&m_server, this);

    qCInfo(categoryCoreBase) << "Registration subscription completed";

    m_server.moveToThread(&m_serverThread);
    m_serverThread.start();
}

