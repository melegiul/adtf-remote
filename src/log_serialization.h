//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
#define UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class LogSerialization {
//public:
    /*void saveLog(QList<QStringList> &logList);
    QList<QStringList> loadLog(QString fileName);*/

public:
    static void delimitFileNumber(QDir &jsonDir);
    static void writeJson(QList<QStringList> &logList, QJsonArray &json);
    static QList<QStringList> readJson(const QByteArray &data);
};

#endif //UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
