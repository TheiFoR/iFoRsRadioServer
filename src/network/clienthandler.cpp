#include "clienthandler.h"

LOG_DECLARE(ClientHandler, Core);
LOG_DECLARE(ClientHandler, Socket);
LOG_DECLARE(ClientHandler, Read);
LOG_DECLARE(ClientHandler, Write);
LOG_DECLARE(ClientHandler, Parse);

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject* parent)
    : UInterface(parent), m_descriptor(socketDescriptor)
{
    qCInfo(categoryClientHandlerCore) << "Create";
}

ClientHandler::~ClientHandler()
{
    qCInfo(categoryClientHandlerCore) << strId() << "ClientHandler destroyed, deleting socket";
}

void ClientHandler::registrationSubscribe()
{
    qCInfo(categoryClientHandlerCore) << strId() << "Registration subscription started";

    registrateTransfer(&m_connectionManager, this);

    emit createSubscribe(api::radio::RadioStationListRequest::__name__, this);
    emit createSubscribe(api::server::ServerConnectionRequest::__name__, this);

    emit subscribe(api::server::ServerConnectionResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));
    emit subscribe(api::radio::RadioStationListResponse::__name__, this, std::bind(&ClientHandler::sendData, this, std::placeholders::_1, std::placeholders::_2));

    registrateTransfer(&m_radioStationsCore, this);
    registrateTransfer(&m_serverStatusCore, this);

    m_registrationCompleted = true;

    qCInfo(categoryClientHandlerCore) << strId() << "Registration subscription completed";
}

void ClientHandler::start()
{
    qCInfo(categoryClientHandlerCore) << strId() << "Starting ClientHandler for socket:" << m_socket;

    m_socket = new QTcpSocket();
    if (!m_socket->setSocketDescriptor(m_descriptor)) {
        qWarning() << "Failed to set socket descriptor";
        delete m_socket;
        emit disconnect(this);
        return;
    }

    m_ip = m_socket->peerAddress().toIPv4Address();
    m_port = m_socket->peerPort();
    m_address = QString::number((m_ip & 0xFF000000) >> 24) + "." + QString::number((m_ip & 0x00FF0000) >> 16) + "." + QString::number((m_ip & 0x0000FF00) >> 8) + "." + QString::number(m_ip & 0x000000FF) + ":" + QString::number(m_port);

    setUseId(true);

    quint64 clientId = quint64(m_ip) * 100000 +  + m_port;
    setId(clientId);
    setStrId("[" + m_address + "]");

    m_radioStationsCore.setId(clientId);
    m_radioStationsCore.setStrId(m_address);
    m_serverStatusCore.setId(clientId);
    m_serverStatusCore.setStrId(m_address);

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    qCInfo(categoryClientHandlerCore) << strId() << "Connections established for socket:" << m_socket;

    parseData();
}

void ClientHandler::onReadyRead() {
    qCInfo(categoryClientHandlerRead) << strId() << "Data received from client";

    if (!m_socket) {
        qCWarning(categoryClientHandlerRead) << strId() << "Error: Socket is not set, cannot read data";
        return;
    }

    m_buffer.append(m_socket->readAll());

    if (!m_registrationCompleted) {
        qCWarning(categoryClientHandlerRead) << strId() << "Registration not completed, cannot process data. Saved to buffer for later processing.";
        return;
    }

    parseData();
}

void ClientHandler::onDisconnected() {
    qCWarning(categoryClientHandlerCore) << strId() << "Client disconnected!";
    removeConnections();
}

void ClientHandler::removalSuccessful(){
    emit disconnected(this);
}

void ClientHandler::sendData(const QString &commandName, const QVariantMap &data)
{
    qCInfo(categoryClientHandlerWrite) << strId() << "Sending data, command:" << commandName << "to" << m_address;

    QVariantMap dataPacket;

    dataPacket["name"] = commandName;
    dataPacket["data"] = data;

    QByteArray dataBytes;
    QByteArray dataSizeBytes;

    QDataStream dataOut(&dataBytes, QIODevice::WriteOnly);

    dataOut << dataPacket;

    QDataStream sizeDataOut(&dataSizeBytes, QIODevice::WriteOnly);

    sizeDataOut << quint64(dataBytes.size());

    qCDebug(categoryClientHandlerWrite) << strId() << "Packet datasize size:" << dataSizeBytes.size();
    qCDebug(categoryClientHandlerWrite) << strId() << "Packet data size:" << dataBytes.size();

    m_socket->write(dataSizeBytes);
    m_socket->flush();
    m_socket->write(dataBytes);
    m_socket->flush();
}

void ClientHandler::parseData()
{
    qCInfo(categoryClientHandlerParse) << strId() << "Parsing data from buffer, current buffer size:" << m_buffer.size();

    if(m_buffer.isEmpty()){
        qCDebug(categoryClientHandlerSocket) << strId() << "Network buffer is empty, nothing to parse";
        return;
    }

    if(m_buffer.size() < sizeof(quint64)){
        qCWarning(categoryClientHandlerParse) << "Buffer size is less than expected size, waiting for more data...";
        return;
    }

    QDataStream in(m_buffer);

    quint64 expectedSize;
    in >> expectedSize;

    if(expectedSize == 0){
        qCWarning(categoryClientHandlerParse) << "Received packet with size 0, waiting for more data...";
        return;
    }
    if(expectedSize > m_buffer.size() - sizeof(quint64)){
        qCWarning(categoryClientHandlerParse) << "Expected packet size is larger than available data, waiting for more data...";
        return;
    }

    qCDebug(categoryClientHandlerParse) << "Expected packet size:" << expectedSize << "| Available data size:" << m_buffer.size() - sizeof(quint64);

    QVariantMap packet;
    in >> packet;

    m_buffer.remove(0, sizeof(quint64) + expectedSize);
    qCDebug(categoryClientHandlerParse) << "Packet received, remaining buffer size:" << m_buffer.size();

    if (!packet.contains("name") || !packet.contains("data")) {
        qCWarning(categoryClientHandlerParse) << "Invalid packet structure";
        return;
    }

    QString commandName = packet["name"].toString();
    QVariantMap data = packet["data"].toMap();

    qCInfo(categoryClientHandlerParse) << "Received command:" << commandName;

    emit signalUCommand(commandName, data);
}
