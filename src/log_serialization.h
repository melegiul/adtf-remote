//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
#define UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <stdexcept>
#include <QVector>
#include <QtCore/QDateTime>
#include <QVariant>

class LogSerialization {
private:
    QDateTime minTime;
    QDateTime maxTime;
    QStringList logLevelFilter;
    QStringList sourceFilter;
    QStringList contextFilter;
    QRegExp payloadFilter;
    void delimitFileNumber(QDir &jsonDir);
    QList<QStringList> readLogData(QJsonArray &logData);

public:
    const QDateTime &getMinTime() const;

    const QDateTime &getMaxTime() const;

    const QStringList &getLogLevelFilter() const;

    const QStringList &getSourceFilter() const;

    const QStringList &getContextFilter() const;

    const QRegExp &getPayloadFilter() const;

private:
    void readFilterData(QJsonObject& wrapperObject);

public:
    LogSerialization(QDateTime minTime, QDateTime maxTime, QStringList logLevelFilter, QStringList sourceFilter,
                     QStringList contextFilter, QRegExp payloadFilter);

    LogSerialization();

    void writeJson(QVector<QStringList> &logVector, QFile &saveFile, int maxFileNumber);
    QList<QStringList> readJson(const QByteArray &data);
};

#endif //UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
