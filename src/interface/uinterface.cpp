#include "uinterface.h"

UInterface::UInterface(QObject *parent)
    : QObject{parent}
{}

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

    QObject::connect(fromUInterface, &UInterface::removed, toUInterface, &UInterface::removed);

    m_interfaces.append(fromUInterface);

    fromUInterface->registrationSubscribe();
}

void UInterface::removedConnections()
{
    for (UInterface* interface : m_interfaces) {
        if (interface->thread() != QThread::currentThread()) {
            QMetaObject::invokeMethod(interface, "removedConnections",
                                      Qt::BlockingQueuedConnection);
        } else {
            interface->removedConnections();
        }
    }
    emit removed(this);
}
