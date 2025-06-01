#include "clienthandler.h"

LOG_DECLARE(ClientHandler, Core);
LOG_DECLARE(ClientHandler, Socket);

ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent)
    : UInterface(parent)
    , m_socket(socket)
{
    qCInfo(categoryClientHandlerCore) << id() << "Create";

    m_ip = m_socket->peerAddress().toIPv4Address();
    m_port = m_socket->peerPort();
    m_address = QString::number((m_ip & 0xFF000000) >> 24) + "." + QString::number((m_ip & 0x00FF0000) >> 16) + "." + QString::number((m_ip & 0x0000FF00) >> 8) + "." + QString::number(m_ip & 0x000000FF) + ":" + QString::number(m_port);

    setUseId(true);

    quint64 clientId = quint64(m_ip) * 100000 +  + m_port;
    setId(clientId);

    m_radioStationsCore.setId(clientId);
    m_serverStatusCore.setId(clientId);
}

ClientHandler::~ClientHandler() {
    return;
    qCInfo(categoryClientHandlerCore) << id() << "ClientHandler destroyed, deleting socket";
    m_socket->deleteLater();
}

void ClientHandler::registrationSubscribe()
{
    qCInfo(categoryClientHandlerCore) << id() << "Registration subscription started";

    emit createSubscribe(api::radio::RadioStationListRequest::__name__, this);
    emit createSubscribe(api::server::ServerConnectionRequest::__name__, this);
    emit subscribe(api::server::ServerConnectionResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));
    emit subscribe(api::radio::RadioStationListResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));

    registrateTransfer(&m_radioStationsCore, this);
    registrateTransfer(&m_serverStatusCore, this);

    qCInfo(categoryClientHandlerCore) << id() << "Registration subscription completed";

    m_registrationComplete = true;

    sendLostPacket();
}

void ClientHandler::start()
{
    qCInfo(categoryClientHandlerCore) << id() << "Starting ClientHandler for socket:" << m_socket;

    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    qCInfo(categoryClientHandlerCore) << id() << "Connections established for socket:" << m_socket;
}

void ClientHandler::onReadyRead() {
    qCInfo(categoryClientHandlerSocket) << id() << "Data received from client:" << m_address;

    qCInfo(categoryClientHandlerSocket) << id() << "New packet!";
    if (!m_socket) {
        qCWarning(categoryClientHandlerSocket) << id() << "Error: Socket is not set, cannot read data";
        return;
    }

    quint64 availableBytes = m_socket->bytesAvailable();

    qCInfo(categoryClientHandlerSocket) << id() << "Available bytes:" << availableBytes;

    while(availableBytes > 0){
        QVariantMap packet;

        if(m_expectedSize == 0){
            QVariantMap sizePacket;

            QByteArray bytes = m_socket->read(m_datasizePacketSize);
            QDataStream in(bytes);
            in >> sizePacket;

            if(!sizePacket.contains("size")){
                qCWarning(categoryClientHandlerSocket) << id() << "Invalid packet structure. Waiting...";
                continue;
            }

            m_expectedSize = sizePacket["size"].toULongLong();
            availableBytes -= m_datasizePacketSize;
            qCInfo(categoryClientHandlerSocket) << id() << "Next packet size:" << m_expectedSize << "|" << availableBytes << " bytes left";
            continue;
        }

        if(availableBytes > m_expectedSize){
            m_buffer.append(m_socket->read(m_expectedSize));
            availableBytes -= m_expectedSize;
        }
        else if(m_buffer.size() + availableBytes > m_expectedSize){
            quint64 size = m_expectedSize - m_buffer.size();
            m_buffer.append(m_socket->read(size));
            availableBytes -= size;
        }
        else{
            m_buffer.append(m_socket->read(availableBytes));
            availableBytes = 0;
        }

        if(m_buffer.size() < m_expectedSize){
            qCInfo(categoryClientHandlerSocket) << id() << "Bytes:" << m_buffer.size() << "/" << m_expectedSize << "|" << m_buffer.size() * 100 / m_expectedSize << "% |" << "Waiting...";
            continue;
        }
        else if(m_buffer.size() == m_expectedSize){
            QDataStream in(m_buffer);
            in >> packet;
            m_buffer.clear();
            m_expectedSize = 0;
            qCInfo(categoryClientHandlerSocket) << id() << "Great full packet received!";
        }
        else{
            qCCritical(categoryClientHandlerSocket) << id() << "FATAL ERROR:" << m_buffer.size() << "/" << m_expectedSize << "|" << m_buffer.size() * 100 / m_expectedSize << "%";
            continue;
        }



        if (!packet.contains("name") || !packet.contains("data")) {
            qCWarning(categoryClientHandlerSocket) << id() << "Invalid packet structure";
            continue;
        }

        QString commandName = packet["name"].toString();
        QVariantMap data = packet["data"].toMap();

        qCDebug(categoryClientHandlerSocket) << id() << "Received command:" << commandName;

        emit signalUCommand(commandName, data);
    }
}

void ClientHandler::onDisconnected() {
    qCWarning(categoryClientHandlerCore) << id() << "Client disconnected!";
    removeConnections();
}

void ClientHandler::removalSuccessful(){
    emit disconnected(m_socket);
}

void ClientHandler::sendData(const QString &commandName, const QVariantMap &data)
{
    qCInfo(categoryClientHandlerSocket) << id() << "Sending data, command:" << commandName << "to" << m_address;

    QVariantMap dataPacket;
    QVariantMap sizePacket;

    dataPacket["name"] = commandName;
    dataPacket["data"] = data;

    QByteArray dataBytes;
    QByteArray dataSizeBytes;

    QDataStream dataOut(&dataBytes, QIODevice::WriteOnly);

    dataOut << dataPacket;

    sizePacket["size"] = quint64(dataBytes.size());

    QDataStream sizeDataOut(&dataSizeBytes, QIODevice::WriteOnly);

    sizeDataOut << sizePacket;

    qCDebug(categoryClientHandlerSocket) << id() << "Packet datasize size:" << dataSizeBytes.size();
    qCDebug(categoryClientHandlerSocket) << id() << "Packet data size:" << dataBytes.size();

    //qCDebug(categoryClientHandlerSocket) << "Raw data to send:" << bytes;
    m_socket->write(dataSizeBytes);
    m_socket->flush();
    m_socket->write(dataBytes);
    m_socket->flush();
}

void ClientHandler::sendLostPacket()
{
    for(const std::pair<const QString&, const QVariantMap&>& packet : m_lostPackets){
        emit signalUPacket(packet.first, packet.second);
        qCInfo(categoryClientHandlerSocket) << id() << "Lost packet sent:" << packet.first;
    }
    m_lostPackets.clear();
}
