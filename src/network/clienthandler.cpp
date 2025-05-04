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

    QByteArray bytes = m_socket->readAll();
    //qCDebug(categoryClientHandlerSocket) << "Raw data received:" << bytes;

    if (bytes.isEmpty()) {
        qCCritical(categoryClientHandlerSocket) << "Received empty data packet!";
        return;
    }

    QDataStream in(bytes);
    QVariantMap packet;
    in >> packet;

    if (packet.isEmpty()) {
        qCCritical(categoryClientHandlerSocket) << "Invalid packet structure!";
        return;
    }

    QString commandName = packet["name"].toString();
    QVariantMap data = packet["data"].toMap();

    qCInfo(categoryClientHandlerSocket) << "Packet received:" << commandName;

    if(m_registrationComplete){
        emit signalUCommand(commandName, data);
        qCInfo(categoryClientHandlerSocket) << "Packet:" << commandName;
    }
    else{
        std::pair<const QString&, const QVariantMap&> packet(commandName, data);
        m_lostPackets.emplace_back(packet);
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
