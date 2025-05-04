#include "radiostationscore.h"

LOG_DECLARE(RadioStationsCore, Core)
LOG_DECLARE(RadioStationsCore, Stations)

RadioStationsCore::RadioStationsCore(QObject *parent)
    : UInterface{parent}
{
    qCInfo(categoryRadioStationsCoreCore) << "Create";

    connect(&m_fileWatcher, &FileWatcher::fileModified, this, &RadioStationsCore::onRadioStationsFileChanged);

    m_fileWatcher.start();
}

void RadioStationsCore::registrationSubscribe()
{
    qCInfo(categoryRadioStationsCoreCore) << "Registration subscription started";

    emit createSubscribe(api::radio::RadioStationListResponse::__name__, this);
    emit subscribe(api::radio::RadioStationListRequest::__name__, this, std::bind(&RadioStationsCore::handleRadioStationsListRequest, this, std::placeholders::_1));

    qCInfo(categoryRadioStationsCoreCore) << "Registration subscription completed";
}

void RadioStationsCore::handleRadioStationsListRequest(const QVariantMap &data)
{
    QVariantMap radioStationsResponse;
    radioStationsResponse[api::radio::RadioStationListResponse::RadioStationList] = QVariantList();

    QVariantMap radioStations = m_jsonStorage.readAll(true);

    if(radioStations.empty()){
        emit signalUPacket(api::radio::RadioStationListResponse::__name__, radioStationsResponse);
        qCWarning(categoryRadioStationsCoreStations) << "Radio station list is empty!";
        return;
    }

    QVariantList radioStationsList;
    for(auto& stationKey : radioStations.keys()){
        QVariantMap radioStationResult;

        QVariantMap radioStation = radioStations[stationKey].toMap();
        QString imagePath = radioStation["image"].toString();
        QFile file(imagePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open image file:" << imagePath;
            return;
        }

        QByteArray imageData = file.readAll();
        file.close();

        radioStationResult[api::radio::RadioStation::Id] = radioStation["id"];
        radioStationResult[api::radio::RadioStation::Name] = radioStation["name"];
        radioStationResult[api::radio::RadioStation::URL] = radioStation["url"];
        radioStationResult[api::radio::RadioStation::Image] = imageData;

        radioStationsList.append(radioStationResult);
    }
    radioStationsResponse[api::radio::RadioStationListResponse::RadioStationList] = radioStationsList;

    emit signalUPacket(api::radio::RadioStationListResponse::__name__, radioStationsResponse);

    qCInfo(categoryRadioStationsCoreStations) << "Radio stations list is sent. Size:" << radioStationsList.size();
}

void RadioStationsCore::onRadioStationsFileChanged()
{
    qCInfo(categoryRadioStationsCoreStations) << "Radio station list has been updated in the file.";
    qCInfo(categoryRadioStationsCoreStations) << "Sending updated radio station list to user...";
;
    handleRadioStationsListRequest({});
}
