#ifndef UINTERFACE_H
#define UINTERFACE_H

#include <QObject>
#include <QVariantMap>
#include <QLoggingCategory>
#include <qeventloop.h>
#include <qthread.h>

#include "api/internal/model.h"
#include "src/types/types.h"
#include "src/types/logdef.h"

using namespace std::placeholders;

class UInterface : public QObject
{
    Q_OBJECT
public:
    explicit UInterface(QObject *parent = nullptr);
    ~UInterface();

    virtual void registrationSubscribe() = 0;
    virtual void removalSuccessful();

    void registrateTransfer(UInterface* fromUInterface, UInterface* toUInterface);

    void removeConnections();

    void setUseId(bool enabled = true);
    bool useId();

    void setId(quint64 id);
    quint64 id();

    QList<UInterface *> childreinIterfaces() const;
    QList<UInterface *> allChildreinIterfaces() const;


signals:
    void signalUCommand(const QString& commandName, const QVariantMap &data);
    void signalUPacket(const QString& commandName, const QVariantMap &data);

    void createSubscribe(const QString& commandName, UInterface* obj);
    void removeSubscribe(const QString& commandName, UInterface* obj);

    void subscribe(const QString& commandName, UInterface* obj, CallbackCommandFunction function);
    void subscribe(const QString& commandName, UInterface* obj, CallbackPacketFunction function);

    void unsubscribe(const QString& commandName, UInterface* obj, CallbackCommandFunction function);
    void unsubscribe(const QString& commandName, UInterface* obj, CallbackPacketFunction function);


    void remove(UInterface * rootObj, QList<UInterface *> objs);


    void signalIdUCommand(const QString& commandName, const QVariantMap &data, quint64 id = 0xFFFFFFFFFFFFFFFF);
    void signalIdUPacket(const QString& commandName, const QVariantMap &data, quint64 id = 0xFFFFFFFFFFFFFFFF);

private slots:
    void onUCommandEmited(const QString& commandName, const QVariantMap &data);
    void onUPacketEmited(const QString& commandName, const QVariantMap &data);

private:
    QList<UInterface*> m_childreinIterfaces;

    bool m_useId = false;
    quint64 m_id = 0xFFFFFFFFFFFFFFFF;
};

#endif // UINTERFACE_H
