#include "filewatcher.h"

FileWatcher::FileWatcher(const QString& filePath, QObject* parent)
    : QObject(parent), m_filePath(filePath) {
    connect(&m_timer, &QTimer::timeout, this, &FileWatcher::checkForModification);
}

void FileWatcher::start(int intervalMs) {
    QFileInfo fileInfo(m_filePath);
    m_lastModified = fileInfo.exists() ? fileInfo.lastModified() : QDateTime();
    m_timer.start(intervalMs);
}

void FileWatcher::stop() {
    m_timer.stop();
}

QString FileWatcher::filePath() const {
    return m_filePath;
}

void FileWatcher::checkForModification() {
    QFileInfo fileInfo(m_filePath);
    if (!fileInfo.exists())
        return;

    QDateTime currentModified = fileInfo.lastModified();
    if (currentModified > m_lastModified) {
        m_lastModified = currentModified;
        emit fileModified();
    }
}
