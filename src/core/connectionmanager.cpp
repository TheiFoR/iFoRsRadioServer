#include "connectionmanager.h"

LOG_DECLARE(ConnectionManager, Core)
LOG_DECLARE(ConnectionManager, Subscribe)
LOG_DECLARE(ConnectionManager, Subscriber)
LOG_DECLARE(ConnectionManager, Command)

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject{parent}
{
    qCInfo(categoryConnectionManagerCore) << "Create";

    connect(this, &ConnectionManager::updateConnections, this, &ConnectionManager::handleUpdateConnections);
}

void ConnectionManager::handleCreateSubscribe(const QString &commandName, UInterface *obj)
{
    qCInfo(categoryConnectionManagerSubscribe) << "Added new command:" << commandName << "from: " << obj;

    m_commandSubscribe[commandName].append(obj);
    emit updateConnections();
}
void ConnectionManager::handleRemoveSubscribe(const QString &commandName, UInterface *obj)
{
    qCInfo(categoryConnectionManagerSubscribe) << "Removed command:" << commandName << "from: " << obj;
}

void ConnectionManager::handleSubscriber(const QString &commandName, UInterface *obj, CallbackPacketFunction function)
{
    qCInfo(categoryConnectionManagerSubscriber) << "Added new packet:" << commandName << "from: " << obj;

    m_packetSubscribers[commandName].append(PacketFunctionContext{obj, function});
    emit updateConnections();
}
void ConnectionManager::handleUnsubscriber(const QString &commandName, UInterface *obj, CallbackPacketFunction function)
{
    qCInfo(categoryConnectionManagerSubscriber) << "Removed packet:" << commandName << "from: " << obj;
}

void ConnectionManager::handleSubscriber(const QString &commandName, UInterface *obj, CallbackCommandFunction function)
{
    qCInfo(categoryConnectionManagerSubscriber) << "Added new command:" << commandName << "from: " << obj;

    m_commandSubscribers[commandName].append(CommandFunctionContext{obj, function});
    emit updateConnections();
}
void ConnectionManager::handleUnsubscriber(const QString &commandName, UInterface *obj, CallbackCommandFunction function)
{
    qCInfo(categoryConnectionManagerSubscriber) << "Removed command:" << commandName << "from: " << obj;
}


void ConnectionManager::handleUpdateConnections()
{
    qCInfo(categoryConnectionManagerCore) << "Update connections";
    for (auto it = m_commandSubscribe.begin(); it != m_commandSubscribe.end(); ++it) {
        const QList<UInterface*>& subscribes = it.value();

        for (UInterface* subscribe : subscribes) {
            connect(subscribe, &UInterface::signalUCommand,
                    this, &ConnectionManager::onCommandReceived,
                    Qt::UniqueConnection);

            connect(subscribe, &UInterface::signalUPacket,
                    this, &ConnectionManager::onPacketReceived,
                    Qt::UniqueConnection);
        }
    }
}

void ConnectionManager::onCommandReceived(const QString& commandName, const QVariantMap& data)
{
    qCDebug(categoryConnectionManagerCommand) << "Command received:" << commandName;
    auto it = m_commandSubscribers.find(commandName);
    if (it != m_commandSubscribers.end()) {
        for (const CommandFunctionContext& ctx : it.value()) {
            if (ctx.obj) {
                QMetaObject::invokeMethod(ctx.obj, [ctx, data]() {
                    ctx.function(data);
                }, Qt::QueuedConnection);
            }
        }
    }
}
void ConnectionManager::onPacketReceived(const QString &commandName, const QVariantMap &data)
{
    auto it = m_packetSubscribers.find(commandName);
    if (it != m_packetSubscribers.end()) {
        for (const PacketFunctionContext& ctx : it.value()) {
            if (ctx.obj) {
                QMetaObject::invokeMethod(ctx.obj, [ctx, commandName, data]() {
                    ctx.function(commandName, data);
                }, Qt::QueuedConnection);
            }
        }
    }
}

