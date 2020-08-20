//
// Created by uniautonom on 15.08.20.
//

#include "log_serialization.h"

/**
 * called when the number of files in the folder exceeds maximum
 * removes file according to fifo
 * @param json folder to delimit the file number
 */
void LogSerialization::delimitFileNumber(QDir &json){
    QStringList fileList = json.entryList(QDir::Files, QDir::Time);
    QString oldestFile = fileList.last();
    json.remove(oldestFile);
}

/**
 * creates for each log entry a json object and appends it to array
 * @param logList log entries from the mainwindow model
 * @param saveFile file, to be written
 */
void LogSerialization::writeJson(QList<QStringList> &logList, QFile &saveFile, int maxFileNumber) {
    QJsonArray json;
    for (QStringList list : logList) {
            QJsonObject logObject;
            logObject["time"] = list.value(0);
            logObject["level"] = list.value(1);
            logObject["source"] = list.value(2);
            logObject["context"] = list.value(3);
            logObject["payload"] = list.value(4);
            json.append(logObject);
    }
    saveFile.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
    QDir jsonDir = QDir(saveFile.fileName().toStdString().data());
    jsonDir.cdUp();
    std::string str = jsonDir.path().toStdString();
    if(jsonDir.entryList(QDir::Files,QDir::NoSort).count() > maxFileNumber){
        delimitFileNumber(jsonDir);
    }
}

/**
 * extracts data to json documents and returns the log file content
 * @param data raw data from file
 * @return list of log entries
 */
QList<QStringList> LogSerialization::readJson(const QByteArray &data) {
    QJsonDocument loadedDoc(QJsonDocument::fromJson(data));
    QJsonArray json = loadedDoc.array();
    QList<QStringList> logList;
    for (int i=0; i<json.size(); i++) {
        QStringList logString;
        QJsonObject logEntry = json[i].toObject();
        if (logEntry.contains("time") && logEntry["time"].isString())
            logString.append(logEntry["time"].toString());
        if (logEntry.contains("level") && logEntry["level"].isString())
            logString.append(logEntry["level"].toString());
        if (logEntry.contains("source") && logEntry["source"].isString())
            logString.append(logEntry["source"].toString());
        if (logEntry.contains("context") && logEntry["context"].isString())
            logString.append(logEntry["context"].toString());
        if (logEntry.contains("payload") && logEntry["payload"].isString())
            logString.append(logEntry["payload"].toString());
        logList.append(logString);
    }
    return logList;
}
