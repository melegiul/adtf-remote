#include <QTcpSocket>
#include <QDataStream>

#include "networkclient.h"
#include "adtfmediasample.h"

NetworkClient::NetworkClient(QObject *parent) : QObject(parent)
{
    connect(&this->tcpSocket, &QTcpSocket::connected, this, &NetworkClient::connected);
    connect(&this->tcpSocket, &QTcpSocket::disconnected, this, &NetworkClient::disconnected);
    connect(&this->tcpSocket, &QIODevice::readyRead, this, &NetworkClient::receive);
    connect(&this->tcpSocket, QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::error), this, &NetworkClient::error);
}

void NetworkClient::connectNetwork(QString host, uint16_t port) {
    if (this->tcpSocket.state() != QTcpSocket::UnconnectedState) {
        qWarning() << "Ignoring connectNetwork() due to network is currently connected";
        return;
    }

    this->tcpSocket.connectToHost(host, port);
}

void NetworkClient::disconnectNetwork() {
    this->tcpSocket.disconnectFromHost();
}

void NetworkClient::send(const ADTFMediaSample &sample)
{
    if (!this->tcpSocket.isWritable()) {
        qWarning() << "No TCP socket or socket not writable. Dropping MediaSample.";
        return;
    }

    QDataStream dataStream(&this->tcpSocket);
    dataStream.setVersion(QDataStream::Qt_4_7);

    dataStream << sample.pinName.data() << sample.mediaType.data() << static_cast<quint64>(sample.streamTime);
    dataStream.writeBytes(sample.data.get(), sample.length);

    this->tcpSocket.flush();
}

void NetworkClient::receive() {
    std::unique_ptr<char[]> pinNamePtr;
    std::unique_ptr<char[]> mediaTypePtr;
    std::unique_ptr<char[]> dataPtr;

    char *pinName, *mediaType, *data;
    quint64 streamTime;
    uint length;

    QDataStream dataStream(&this->tcpSocket);
    dataStream.setVersion(QDataStream::Qt_4_7);

    while (!dataStream.atEnd()) {
        dataStream.startTransaction();
        dataStream >> pinName >> mediaType >> streamTime;
        dataStream.readBytes(data, length);

        pinNamePtr.reset(pinName);
        mediaTypePtr.reset(mediaType);
        dataPtr.reset(data);

        if (!dataStream.commitTransaction())
            return;

        ADTFMediaSample sample;
        sample.pinName = pinName;
        sample.mediaType = mediaType;
        sample.streamTime = streamTime;
        sample.data = std::move(dataPtr);
        sample.length = length;

        emit received(sample);
    }
}

void NetworkClient::error() {
   emit errored(QString("Disconnected due to error: %1").arg(this->tcpSocket.errorString()));
}
