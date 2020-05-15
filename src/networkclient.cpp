//#include <string>
//#include <memory>
#include <QSettings>
#include <QTcpSocket>

#include "networkclient.h"

#include "tmyadtfmessage.h"

NetworkClient::NetworkClient(QObject *parent) : QObject(parent), dataStream(&this->tcpSocket)
{
    this->dataStream.setVersion(QDataStream::Qt_4_7);
    connect(this->tcpSocket, &QIODevice::readyRead, this, &NetworkClient::receive);
    connect(this->tcpSocket, QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::error), this, &MainWindow::displayError);
}

void NetworkClient::connectNetwork(QString host, uint16_t port) {
    if (this->tcpSocket == nullptr) {
        this->tcpSocket = new QTcpSocket(this);
        this->dataStream.setDevice(this->tcpSocket);

    }

    if (this->tcpSocket->state() != QTcpSocket::UnconnectedState) {
        qWarning() << "Activating connectToNetwork() slot shouldn't be possible as network is currently connected";
        return;
    }

    this->tcpSocket->connectToHost(host, port);
}

void NetworkClient::disconnectNetwork() {
    this->tcpSocket->disconnectFromHost();
}

void NetworkClient::send(const ADTFMediaSample &sample)
{
    if (this->tcpSocket == nullptr || !this->tcpSocket->isWritable()) {
        qWarning() << "No TCP socket or socket not writable. Dropping MediaSample.";
        return;
    }

    this->dataStream << sample.pinName.data() << sample.mediaType.data() << static_cast<quint64>(sample.streamTime);
    this->dataStream.writeBytes(sample.data.get(), sample.length);

    this->tcpSocket->flush();
}

void NetworkClient::receive() {
    // TODO test network fragmentation
    std::unique_ptr<char[]> pinNamePtr;
    std::unique_ptr<char[]> mediaTypePtr;
    std::unique_ptr<char[]> dataPtr;

    char *pinName, *mediaType, *data;
    quint64 streamTime;
    uint length;

    this->dataStream.startTransaction();
    this->dataStream >> pinName >> mediaType >> streamTime;
    this->dataStream.readBytes(data, length);

    pinNamePtr.reset(pinName);
    mediaTypePtr.reset(mediaType);
    dataPtr.reset(data);

    qDebug() << "Message received for pin" << pinName << "at time" << streamTime << "of length" << length << "with media type" << mediaType;

    if (!this->dataStream.commitTransaction())
        return;

    ADTFMediaSample sample;
    sample.pinName = pinName;
    sample.mediaType = mediaType;
    sample.streamTime = streamTime;
    sample.data = std::move(dataPtr);
    sample.length = length;

    emit received(sample);
}
