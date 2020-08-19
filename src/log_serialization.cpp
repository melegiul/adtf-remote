//
// Created by uniautonom on 15.08.20.
//

#include "log_serialization.h"

/**
 * first retrieves settings for predefined memory location
 * then writes to json file and ensures the max file number limitation
 * @param logList list of current log entries in the mainwindow model
 *//*
void LogSerialization::saveLog(QList<QStringList> &logList){

    QSettings settings;
    QString qLogPath = settings.value("logview/logPath").toString();
    std::string logPath = qLogPath.toStdString();
    time_t now = time(0);
    char fileName[40];
    tm *tm_info = localtime(&now);
    strftime(fileName, 26, "/%Y-%m-%d-%H:%M:%S.json", tm_info);
    std::string filePath = logPath + fileName;
    QFile saveFile(filePath.data());
    if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning("log.cpp-saveLog(): Could not open save file.");
        return;
    }
    QJsonArray json;
    writeJson(logList, json);


    saveFile.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
    saveFile.close();
    QDir jsonDir = QDir(logPath.data());
    if(jsonDir.entryList(QDir::Files,QDir::NoSort).count() > 10){
        delimitFileNumber(jsonDir);
    }
}*/

/**
 * called when the number of files in the folder exceeds maximum
 * removes file according to fifo
 * @param json folder to delimit the file number
 */
void LogSerialization::delimitFileNumber(QDir &json){
    QStringList fileList = json.entryList(QDir::Files, QDir::Name);
    QString oldestFile = fileList.first();
    json.remove(oldestFile);
}

/**
 * creates for each log entry a json object and appends it to array
 * @param logList log entries from the mainwindow model
 * @param json resulting json Objects are placed in array
 */
void LogSerialization::writeJson(QList<QStringList> &logList, QJsonArray &json){
    for (QStringList list : logList) {
            QJsonObject logObject;
            logObject["time"] = list.value(0);
            logObject["level"] = list.value(1);
            logObject["source"] = list.value(2);
            logObject["context"] = list.value(3);
            logObject["payload"] = list.value(4);
            json.append(logObject);
    }
}

/*QList<QStringList> LogSerialization::loadLog(QString fileName){
    QFile jsonFile(fileName);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning("log.cpp-loadLog(): Could not open file");
    }
    QByteArray data = jsonFile.readAll();
    jsonFile.close();
    QJsonDocument loadedDoc(QJsonDocument::fromJson(data));
    return readJson(loadedDoc.array());
}*/

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
