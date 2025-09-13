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
    // qCInfo(categoryConnectionManagerSubscriber) << "Removed packet:" << commandName << "from: " << obj;
}

std::unordered_set<UInterface*> ConnectionManager::collectChildrenRecursive(UInterface* root)
{
    std::unordered_set<UInterface*> result;
    if (!root) return result;

    std::vector<UInterface*> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        UInterface* current = stack.back();
        stack.pop_back();

        if (!current || result.count(current))
            continue;

        result.insert(current);

        const auto& childs = current->children();
        for (QObject* childObj : childs) {
            if (auto child = qobject_cast<UInterface*>(childObj)) {
                stack.push_back(child);
            }
        }
    }

    return result;
}

void ConnectionManager::onRemoved(UInterface * rootObj)
{
    qCInfo(categoryConnectionManagerCore) << "Removing all connections for object:" << rootObj;

    std::unordered_set<UInterface*> allChildren = collectChildrenRecursive(rootObj);

    for (UInterface* obj : allChildren) {
        qCDebug(categoryConnectionManagerCore) << " - " << obj;
    }

    for (auto it = m_commandSubscribe.begin(); it != m_commandSubscribe.end(); ++it) {
        auto& list = it.value(); // QList<UInterface*>
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&](UInterface* obj){ return allChildren.count(obj) > 0; }),
                   list.end());
    }

    for (auto it = m_commandSubscribers.begin(); it != m_commandSubscribers.end(); ++it) {
        auto& list = it.value(); // QList<CommandFunctionContext>
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&](const CommandFunctionContext& ctx){ return allChildren.count(ctx.obj) > 0; }),
                   list.end());
    }

    for (auto it = m_packetSubscribers.begin(); it != m_packetSubscribers.end(); ++it) {
        auto& list = it.value(); // QList<PacketFunctionContext>
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&](const PacketFunctionContext& ctx){ return allChildren.count(ctx.obj) > 0; }),
                   list.end());
    }

    qCInfo(categoryConnectionManagerCore) << "Removed" << allChildren.size() << "connections.";
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
            connect(subscribe, &UInterface::signalIdUCommand,
                    this, &ConnectionManager::onCommandReceived,
                    Qt::UniqueConnection);

            connect(subscribe, &UInterface::signalIdUPacket,
                    this, &ConnectionManager::onPacketReceived,
                    Qt::UniqueConnection);
        }
    }
}

void ConnectionManager::onCommandReceived(const QString& commandName, const QVariantMap& data, quint64 id)
{
    // qCDebug(categoryConnectionManagerCommand) << "Command received:" << commandName;
    auto it = m_commandSubscribers.find(commandName);
    if (it != m_commandSubscribers.end()) {
        for (const CommandFunctionContext& ctx : it.value()) {
            if (ctx.obj && (!ctx.obj->useId() || (ctx.obj->id() == id))) {
                QMetaObject::invokeMethod(ctx.obj, [ctx, data]() {
                    ctx.function(data);
                }, Qt::QueuedConnection);
            }
            else {
                // qDebug(categoryConnectionManagerCommand)
                // << "Skipping invoke coommand:"
                // << "ctx.obj=" << ctx.obj
                // << (ctx.obj ? QString("ctx.obj->useId()=%1").arg(ctx.obj->useId()) : "ctx.obj->useId()=N/A")
                // << (ctx.obj ? QString("ctx.obj->id()=%1").arg(ctx.obj->id()) : "ctx.obj->id()=N/A")
                // << "id=" << id;
            }
        }
    }
}
void ConnectionManager::onPacketReceived(const QString &commandName, const QVariantMap &data, quint64 id)
{
    qCDebug(categoryConnectionManagerCommand) << "Packet received:" << commandName;
    auto it = m_packetSubscribers.find(commandName);
    if (it != m_packetSubscribers.end()) {
        for (const PacketFunctionContext& ctx : it.value()) {
            if (ctx.obj && (!ctx.obj->useId() || (ctx.obj->id() == id))) {
                QMetaObject::invokeMethod(ctx.obj, [ctx, commandName, data]() {
                    ctx.function(commandName, data);
                }, Qt::QueuedConnection);
            }
            else {
                // qDebug(categoryConnectionManagerCommand)
                // << "Skipping invoke packet:"
                // << "ctx.obj=" << ctx.obj
                // << (ctx.obj ? QString("ctx.obj->useId()=%1").arg(ctx.obj->useId()) : "ctx.obj->useId()=N/A")
                // << (ctx.obj ? QString("ctx.obj->id()=%1").arg(ctx.obj->id()) : "ctx.obj->id()=N/A")
                // << "id=" << id;
            }
        }
    }
}

