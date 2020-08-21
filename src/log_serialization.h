//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
#define UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

class LogSerialization {
private:
    static void delimitFileNumber(QDir &jsonDir);
    /*void saveLog(QList<QStringList> &logList);
    QList<QStringList> loadLog(QString fileName);*/

public:
    static void writeJson(QList<QStringList> &logList, QFile &saveFile, int maxFileNumber, QString defaultPath);
    static QList<QStringList> readJson(const QByteArray &data);
};

#endif //UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
