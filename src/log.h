//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_H
#define UNIAUTONOM_REMOTEGUI_LOG_H

#include <QDir>
#include <QSettings>
#include <QStringListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class Log {
public:
    void saveLog(QStringList logList);

private:
    void delimitFileNumber(QDir &jsonDir);
    void writeJson(QStringList logList, QJsonArray &json);
    void readJson();
};

#endif //UNIAUTONOM_REMOTEGUI_LOG_H
