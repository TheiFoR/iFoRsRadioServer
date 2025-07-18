#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QLoggingCategory>
#include <QFileInfo>

#include "src/types/logdef.h"


class FileManager : public QObject
{
    Q_OBJECT

    LOG_DECLARE(FileManager, Read);
    LOG_DECLARE(FileManager, Write);

public:
    explicit FileManager(QObject *parent = nullptr);

    QByteArray read(const QString &filePath);
    bool write(const QString &filePath, const QString &data);

signals:
};

#endif // FILEMANAGER_H
