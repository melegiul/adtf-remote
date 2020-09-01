//
// Created by uniautonom on 15.08.20.
//

#include "log_serialization.h"

/**
 * called when the number of files in the user defined folder for automatic save exceeds maximum
 * removes file according to fifo, e.g. oldest entry removed first
 * @param json folder to delimit the file number
 */
void LogSerialization::delimitFileNumber(QDir &json){
    QStringList fileList = json.entryList(QDir::Files, QDir::Time);
    QString oldestFile = fileList.last();
    json.remove(oldestFile);
}

/**
 * creates for each log entry a json object and appends it to array
 * @param logVector log entries from the mainwindow model
 * @param saveFile file, to be written
 */
void LogSerialization::writeJson(QVector<QStringList> &logVector, QFile &saveFile, int maxFileNumber) {
    QJsonArray json;
    QFileInfo fileInfo = QFileInfo(saveFile);
    QDir fileDir = fileInfo.absoluteDir();
    for (QStringList list : logVector) {
            QJsonObject logObject;
            logObject["time"] = list.value(0);
            logObject["level"] = list.value(1);
            logObject["source"] = list.value(2);
            logObject["context"] = list.value(3);
            logObject["payload"] = list.value(4);
            json.append(logObject);
    }
    saveFile.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
    if (fileDir.entryList(QDir::Files, QDir::NoSort).count() > maxFileNumber) {
        delimitFileNumber(fileDir);
    }
}

/**
 * extracts data to json documents and returns the log file content
 * @param data raw data from file
 * @return list of log entries
 */
QList<QStringList> LogSerialization::readJson(const QByteArray &data) {
    QJsonDocument loadedDoc;
    QJsonParseError error;
    if (!QJsonDocument::fromJson(data, &error).isNull()) {
        loadedDoc = QJsonDocument::fromJson(data);
    } else {
        std::string message = error.errorString().toStdString();
        throw std::runtime_error("Error parsing json file: " + message);
    }
    QJsonArray json = loadedDoc.array();
    if (json.isEmpty()) {
        throw std::runtime_error("Json document contains an object, but an array was expected");
    }
    QList<QStringList> logList;
    for (int i=0; i<json.size(); i++) {
        QStringList logString;
        QJsonObject logEntry = json[i].toObject();
        if (logEntry.contains("time") && logEntry["time"].isString())
            logString.append(logEntry["time"].toString());
        else
            throw std::runtime_error("Json objects do not contain key time with string value");
        if (logEntry.contains("level") && logEntry["level"].isString())
            logString.append(logEntry["level"].toString());
        else
            throw std::runtime_error("Json objects do not contain key level with string value");
        if (logEntry.contains("source") && logEntry["source"].isString())
            logString.append(logEntry["source"].toString());
        else
            throw std::runtime_error("Json objects do not contain key source with string value");
        if (logEntry.contains("context") && logEntry["context"].isString())
            logString.append(logEntry["context"].toString());
        else
            throw std::runtime_error("Json objects do not contain key context with string value");
        if (logEntry.contains("payload") && logEntry["payload"].isString())
            logString.append(logEntry["payload"].toString());
        else
            throw std::runtime_error("Json objects do not contain key payload with string value");
        logList.prepend(logString);
    }
    return logList;
}
