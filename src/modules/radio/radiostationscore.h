#ifndef RADIOSTATIONSCORE_H
#define RADIOSTATIONSCORE_H

#include <QFile>

#include "src/interface/uinterface.h"

#include "api/external/radio.h"
#include "src/utils/jsonstorage.h"

class RadioStationsCore : public UInterface
{
    Q_OBJECT
public:
    explicit RadioStationsCore(QObject *parent = nullptr);

    void registrationSubscribe() override;

private:
    JsonStorage m_jsonStorage{"RadioStations/radio_stations.json"};

    void handleRadioStationsListRequest(const QVariantMap& data);
};

#endif // RADIOSTATIONSCORE_H
