#ifndef __NETWORKCLIENT_H
#define __NETWORKCLIENT_H

#include <memory>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

struct ADTFMediaSample {
    std::string pinName;
    std::string mediaType;
    uint64_t streamTime;
    std::shared_ptr<char[]> data;
    uint length;
};

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);

public slots:
    void connectNetwork(QString host, uint16_t port);
    void disconnectNetwork();
    void send(const ADTFMediaSample &mediaSample);
    void receive();

signals:
    void connected();
    void disconnected();
    void received(ADTFMediaSample sample);

private:
    QTcpSocket tcpSocket;
    QDataStream dataStream;
};

#endif // __NETWORKCLIENT_H
