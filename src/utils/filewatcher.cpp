#include "filewatcher.h"

LOG_DECLARE(FileWatcher, Core)
LOG_DECLARE(FileWatcher, Watch)

FileWatcher::FileWatcher(const QString& filePath, QObject* parent)
    : QObject(parent), m_filePath(filePath) {
    connect(&m_timer, &QTimer::timeout, this, &FileWatcher::checkForModification);
}

void FileWatcher::start(int intervalMs) {
    QFileInfo fileInfo(m_filePath);
    m_lastModified = fileInfo.exists() ? fileInfo.lastModified() : QDateTime();
    m_timer.start(intervalMs);

    qCInfo(categoryFileWatcherWatch) << "Started watching file:" << m_filePath << "with interval:" << intervalMs << "ms";
}

void FileWatcher::stop() {
    m_timer.stop();
}

QString FileWatcher::filePath() const {
    return m_filePath;
}

void FileWatcher::checkForModification() {
    QFileInfo fileInfo(m_filePath);
    if (!fileInfo.exists()){
        qCWarning(categoryFileWatcherWatch) << "File does not exist:" << m_filePath;
        return;
    }

    QDateTime currentModified = fileInfo.lastModified();
    if (currentModified > m_lastModified) {
        m_lastModified = currentModified;
        qCInfo(categoryFileWatcherWatch) << "File modified:" << m_filePath;
        emit fileModified();
    }
}
