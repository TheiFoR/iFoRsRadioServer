#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QString>

class FileWatcher : public QObject {
    Q_OBJECT

public:
    explicit FileWatcher(const QString& filePath, QObject* parent = nullptr);

    void start(int intervalMs = 10000);
    void stop();

    QString filePath() const;

signals:
    void fileModified();

private slots:
    void checkForModification();

private:
    QString m_filePath;
    QDateTime m_lastModified;
    QTimer m_timer;
};

#endif // FILEWATCHER_H
