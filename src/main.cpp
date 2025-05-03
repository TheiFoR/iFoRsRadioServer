#include <QCoreApplication>
#include <QDebug>

#include "core/core.h"
#include "utils/config.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "[%s] %s\n", context.category, localMsg.constData());
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);
    QCoreApplication a(argc, argv);

    Config::setConfigFilePath("config.ini");

    Core core;
    core.registrationSubscribe();

    return a.exec();
}
