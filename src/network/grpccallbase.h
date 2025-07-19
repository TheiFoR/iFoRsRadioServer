#ifndef GRPCCALLBASE_H
#define GRPCCALLBASE_H

#include <QObject>

class GrpcCallBase : public QObject
{
    Q_OBJECT
public:
    virtual ~GrpcCallBase() = default;

    virtual void proceed(bool ok) = 0;

    enum CallStatus {
        CREATE,
        PROCESS,
        FINISH
    };

public slots:
    virtual void onFinished(const QVariantMap& data) = 0;

signals:
    void proccess(const QString& commandName, const QVariantMap& data);
};

#endif // GRPCCALLBASE_H
