#include "filemanager.h"

FileManager::FileManager(QObject *parent)
    : QObject{parent}
{}

QByteArray FileManager::read(const QString &filePath) {
    if (filePath.isEmpty()) {
        qCWarning(categoryFileManagerRead) << "File path is empty";
        return QByteArray();
    }
    if (!QFile::exists(filePath)) {
        qCWarning(categoryFileManagerRead) << "File does not exist:" << filePath;
        return QByteArray();
    }
    if (!QFileInfo(filePath).isFile()) {
        qCWarning(categoryFileManagerRead) << "Path is not a file:" << filePath;
        return QByteArray();
    }
    if (!QFileInfo(filePath).isReadable()) {
        qCWarning(categoryFileManagerRead) << "File is not readable:" << filePath;
        return QByteArray();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(categoryFileManagerRead) << "Could not open file for reading:" << filePath;
        return QByteArray();
    }

    return file.readAll();
}

bool FileManager::write(const QString &filePath, const QString &data) {
    if (filePath.isEmpty()) {
        qCWarning(categoryFileManagerWrite) << "File path is empty";
        return false;
    }
    if (!QFile::exists(filePath)) {
        qCWarning(categoryFileManagerWrite) << "File does not exist:" << filePath;
        return false;
    }
    if (!QFileInfo(filePath).isFile()) {
        qCWarning(categoryFileManagerWrite) << "Path is not a file:" << filePath;
        return false;
    }
    if (data.isEmpty()) {
        qCWarning(categoryFileManagerWrite) << "Data to write is empty";
        return false;
    }
    if (!QFileInfo(filePath).isWritable()) {
        qCWarning(categoryFileManagerWrite) << "File is not writable:" << filePath;
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCWarning(categoryFileManagerWrite) << "Could not open file for writing:" << filePath;
        return false;
    }

    file.write(data.toUtf8());
    file.close();

    return true;
}
