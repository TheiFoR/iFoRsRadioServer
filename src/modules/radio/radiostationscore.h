#ifndef RADIOSTATIONSCORE_H
#define RADIOSTATIONSCORE_H

#include <QFile>

#include "api/external/radio.h"

#include "src/interface/uinterface.h"
#include "src/utils/filewatcher.h"
#include "src/utils/jsonstorage.h"

class RadioStationsCore : public UInterface
{
    Q_OBJECT
public:
    explicit RadioStationsCore(QObject *parent = nullptr);

    void registrationSubscribe() override;

private:
    const QString m_filePath = "RadioStations/radio_stations.json";

    JsonStorage m_jsonStorage{m_filePath};
    FileWatcher m_fileWatcher{m_filePath};

    void handleRadioStationsListRequest(const QVariantMap& data);
    void onRadioStationsFileChanged();
};

#endif // RADIOSTATIONSCORE_H
