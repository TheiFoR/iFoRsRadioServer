#include "radiostationscore.h"

LOG_DECLARE(RadioStationsCore, Core)
LOG_DECLARE(RadioStationsCore, Stations)

RadioStationsCore::RadioStationsCore(QObject *parent)
    : UInterface{parent}
{
    qCInfo(categoryRadioStationsCoreCore) << "Create";
    setUseId(true);

    connect(&m_fileWatcher, &FileWatcher::fileModified, this, &RadioStationsCore::onRadioStationsFileChanged);

    // Check if path and file exists, if not create an empty one
    QFile file(m_filePath);
    if (!file.exists()) {
        QFileInfo fileInfo(m_filePath);
        QDir dir = fileInfo.dir();
        if (!dir.exists()) {
            if (dir.mkpath(".")) {
                qCInfo(categoryRadioStationsCoreStations) << "Created directory for radio stations:" << dir.absolutePath();
            } else {
                qCCritical(categoryRadioStationsCoreStations) << "Failed to create directory for radio stations:" << dir.absolutePath();
            }
        }

        if (file.open(QIODevice::WriteOnly)) {
            file.write("{}");
            file.close();
            qCInfo(categoryRadioStationsCoreStations) << "Created empty radio stations file:" << m_filePath;
        } else {
            qCCritical(categoryRadioStationsCoreStations) << "Failed to create radio stations file:" << m_filePath;
        }
    }

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

    QVariantList radioStationsList = radioStations["radio_stations"].toList();
    QVariantList radioStationListResult;
    for(const QVariant& radioStation :radioStationsList){
        const QVariantMap& radioStationMap = radioStation.toMap();

        QVariantMap radioStationMapResult;

        radioStationMapResult[api::radio::RadioStation::Id] = radioStationMap["id"];
        radioStationMapResult[api::radio::RadioStation::Name] = radioStationMap["name"];
        radioStationMapResult[api::radio::RadioStation::URL] = radioStationMap["url"];
        radioStationMapResult[api::radio::RadioStation::Image] = radioStationMap["image"];

        radioStationListResult.append(radioStationMapResult);
    }
    radioStationsResponse[api::radio::RadioStationListResponse::RadioStationList] = radioStationListResult;

    emit signalUPacket(api::radio::RadioStationListResponse::__name__, radioStationsResponse);

    qCInfo(categoryRadioStationsCoreStations) << "Radio stations list is sent. Size:" << radioStationListResult.size();
}

void RadioStationsCore::onRadioStationsFileChanged()
{
    qCInfo(categoryRadioStationsCoreStations) << "Radio station list has been updated in the file.";
    qCInfo(categoryRadioStationsCoreStations) << "Sending updated radio station list to user...";
;
    handleRadioStationsListRequest({});
}
