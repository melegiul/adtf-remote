//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
#define UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H

#include <QDir>
#include <QSettings>
#include <QStringListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardItemModel>

class LogSerialization {
public:
    void saveLog(QList<QStringList> &logList);
    QList<QStringList> loadLog(QString fileName);

private:
    void delimitFileNumber(QDir &jsonDir);
    void writeJson(QList<QStringList> &logList, QJsonArray &json);
    QList<QStringList> readJson(const QJsonArray &json);
};

#endif //UNIAUTONOM_REMOTEGUI_LOG_SERIALIZATION_H
