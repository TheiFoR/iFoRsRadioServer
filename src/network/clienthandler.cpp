#include "clienthandler.h"

LOG_DECLARE(ClientHandler, Core);
LOG_DECLARE(ClientHandler, Socket);

ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent)
    : UInterface(parent)
    , m_socket(socket)
{
    qCInfo(categoryClientHandlerCore) << "Create:" << m_socket;
}

ClientHandler::~ClientHandler() {
    qCInfo(categoryClientHandlerCore) << "ClientHandler destroyed, deleting socket";
    m_socket->deleteLater();
}

void ClientHandler::registrationSubscribe()
{
    qCInfo(categoryClientHandlerCore) << "Registration subscription started";

    emit createSubscribe(api::radio::RadioStationListRequest::__name__, this);
    emit createSubscribe(api::server::ServerConnectionRequest::__name__, this);
    emit subscribe(api::server::ServerConnectionResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));
    emit subscribe(api::radio::RadioStationListResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));

    registrateTransfer(&m_radioStationsCore, this);
    registrateTransfer(&m_serverStatusCore, this);

    qCInfo(categoryClientHandlerCore) << "Registration subscription completed";

    m_registrationComplete = true;

    sendLostPacket();
}

void ClientHandler::start()
{
    qCInfo(categoryClientHandlerCore) << "Starting ClientHandler for socket:" << m_socket;

    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    qCInfo(categoryClientHandlerCore) << "Connections established for socket:" << m_socket;
}

void ClientHandler::onReadyRead() {
    qCInfo(categoryClientHandlerSocket) << "Data received from client";

    qCInfo(categoryClientHandlerSocket) << "New packet!";
    if (!m_socket) {
        qCWarning(categoryClientHandlerSocket) << "Error: Socket is not set, cannot read data";
        return;
    }

    quint64 availableBytes = m_socket->bytesAvailable();

    qCInfo(categoryClientHandlerSocket) << "Available bytes:" << availableBytes;

    while(availableBytes > 0){
        QVariantMap packet;

        if(m_expectedSize == 0){
            QVariantMap sizePacket;

            QByteArray bytes = m_socket->read(m_datasizePacketSize);
            QDataStream in(bytes);
            in >> sizePacket;

            if(!sizePacket.contains("size")){
                qCWarning(categoryClientHandlerSocket) << "Invalid packet structure. Waiting...";
                continue;
            }

            m_expectedSize = sizePacket["size"].toULongLong();
            availableBytes -= m_datasizePacketSize;
            qCInfo(categoryClientHandlerSocket) << "Next packet size:" << m_expectedSize << "|" << availableBytes << " bytes left";
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
            qCInfo(categoryClientHandlerSocket) << "Bytes:" << m_buffer.size() << "/" << m_expectedSize << "|" << m_buffer.size() * 100 / m_expectedSize << "% |" << "Waiting...";
            continue;
        }
        else if(m_buffer.size() == m_expectedSize){
            QDataStream in(m_buffer);
            in >> packet;
            m_buffer.clear();
            m_expectedSize = 0;
            qCInfo(categoryClientHandlerSocket) << "Great full packet received!";
        }
        else{
            qCCritical(categoryClientHandlerSocket) << "ERROR ---> :" << m_buffer.size() << "/" << m_expectedSize << "|" << m_buffer.size() * 100 / m_expectedSize << "%";
            continue;
        }



        if (!packet.contains("name") || !packet.contains("data")) {
            qCWarning(categoryClientHandlerSocket) << "Invalid packet structure";
            continue;
        }

        QString commandName = packet["name"].toString();
        QVariantMap data = packet["data"].toMap();

        qCDebug(categoryClientHandlerSocket) << "Received command:" << commandName;

        emit signalUCommand(commandName, data);
    }
}

void ClientHandler::onDisconnected() {
    qCWarning(categoryClientHandlerCore) << "Client disconnected!";
    removedConnections();
    emit disconnected(this);
}

void ClientHandler::sendData(const QString &commandName, const QVariantMap &data)
{
    qCInfo(categoryClientHandlerSocket) << "Sending data, command:" << commandName;

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

    qCDebug(categoryClientHandlerSocket) << "Packet datasize size:" << dataSizeBytes.size();
    qCDebug(categoryClientHandlerSocket) << "Packet data size:" << dataBytes.size();

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
        qCInfo(categoryClientHandlerSocket) << "Lost packet sent:" << packet.first;
    }
    m_lostPackets.clear();
}
