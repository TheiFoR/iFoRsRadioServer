#include "uinterface.h"

UInterface::UInterface(QObject *parent)
    : QObject{parent}
{
    setId(quint64(this));

    connect(this, &UInterface::signalUCommand, this, &UInterface::onUCommandEmited);
    connect(this, &UInterface::signalUPacket, this, &UInterface::onUPacketEmited);
}

UInterface::~UInterface()
{

}

void UInterface::registrateTransfer(UInterface *fromUInterface, UInterface *toUInterface)
{
    QObject::connect(fromUInterface, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::subscribe), toUInterface, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::subscribe));
    QObject::connect(fromUInterface, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::subscribe), toUInterface, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::subscribe));

    QObject::connect(fromUInterface, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::unsubscribe), toUInterface, QOverload<const QString&, UInterface*, CallbackCommandFunction>::of(&UInterface::unsubscribe));
    QObject::connect(fromUInterface, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::unsubscribe), toUInterface, QOverload<const QString&, UInterface*, CallbackPacketFunction>::of(&UInterface::unsubscribe));

    QObject::connect(fromUInterface, &UInterface::createSubscribe, toUInterface, &UInterface::createSubscribe);
    QObject::connect(fromUInterface, &UInterface::removeSubscribe, toUInterface, &UInterface::removeSubscribe);

    QObject::connect(fromUInterface, &UInterface::remove, toUInterface, &UInterface::remove);

    m_childreinIterfaces.append(fromUInterface);

    fromUInterface->registrationSubscribe();
}

void UInterface::removeConnections()
{
    emit remove(this, allChildreinIterfaces());
}

void UInterface::setUseId(bool enabled)
{
    m_useId = enabled;
}

bool UInterface::useId()
{
    return m_useId;
}

void UInterface::setId(quint64 id)
{
    m_id = id;
}

quint64 UInterface::id()
{
    return m_id;
}

void UInterface::onUCommandEmited(const QString &commandName, const QVariantMap &data)
{
    emit signalIdUCommand(commandName, data, m_id);
}

void UInterface::onUPacketEmited(const QString &commandName, const QVariantMap &data)
{
    emit signalIdUPacket(commandName, data, m_id);
}

QList<UInterface *> UInterface::childreinIterfaces() const
{
    return m_childreinIterfaces;
}

QList<UInterface *> UInterface::allChildreinIterfaces() const
{
    QList<UInterface *> allChildrenInterfaces;
    for (UInterface* interface : m_childreinIterfaces) {
        QList<UInterface *> allSubChildrenInterfaces = interface->allChildreinIterfaces();
        allChildrenInterfaces.append(allSubChildrenInterfaces);
    }
    allChildrenInterfaces.append(childreinIterfaces());
    return allChildrenInterfaces;
}

void UInterface::removalSuccessful(){}
