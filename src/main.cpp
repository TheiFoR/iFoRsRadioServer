#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QMutex>

#include "core/core.h"
#include "utils/config.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static int maxCategoryWidth = 0;
    static int maxSubcategoryWidth = 0;
    static QMutex mutex;

    QStringList parts = QString(context.category).split("][");
    QString category = parts.value(0);
    QString subcategory = parts.value(1);

    {
        QMutexLocker locker(&mutex);
        if (category.length() > maxCategoryWidth)
            maxCategoryWidth = category.length();
        if (subcategory.length() > maxSubcategoryWidth)
            maxSubcategoryWidth = subcategory.length();
    }

    auto centerText = [](const QString& text, int width) -> QString {
        int padding = width - text.length();
        int left = padding / 2;
        int right = padding - left;
        return QString(left, ' ') + text + QString(right, ' ');
    };

    QString levelChar;
    QString msgColor;
    switch (type) {
    case QtDebugMsg:
        levelChar = "\033[37mD\033[0m";
        msgColor = "\033[0;37m";
        break; // Blue
    case QtInfoMsg:
        levelChar = "\033[34mI\033[0m";
        msgColor = "\033[0;34m";
        break; // Green
    case QtWarningMsg:
        levelChar = "\033[33mW\033[0m";
        msgColor = "\033[1;33m";
        break; // Yellow
    case QtCriticalMsg:
        levelChar = "\033[31mE\033[0m";
        msgColor = "\033[1;31m";
        break; // Red
    case QtFatalMsg:
        levelChar = "\033[41mF\033[0m";
        msgColor = "\033[41m";
        break; // Red background
    default:
        levelChar = "\033[37m?\033[0m";
        break; // White
    }

    QString time = QTime::currentTime().toString("HH:mm:ss.zzz");

    QString formattedCategory, formattedSubcategory;
    {
        QMutexLocker locker(&mutex);
        formattedCategory = centerText(category, maxCategoryWidth) + "][" + centerText(subcategory, maxSubcategoryWidth);
    }

    QTextStream(stderr)
        << "\033[0;37m[" << time << "]\033[0m "   // Gray time
        << levelChar << " "
        << "\033[1;37m[" << formattedCategory << "]\033[0m "
        << msgColor << msg << "\033[0m"       // Light gray message
        << '\n';
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
