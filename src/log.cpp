//
// Created by uniautonom on 15.08.20.
//

#include "log.h"

/**
 * first retrieves settings for predefined memory location
 * then writes to json file and ensures the max file number limitation
 * @param logList list of current log entries in the mainwindow model
 */
void Log::saveLog(QStringList logList){
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
}

/**
 * called when the number of files in the folder exceeds maximum
 * removes file according to fifo
 * @param json folder to delimit the file number
 */
void Log::delimitFileNumber(QDir &json){
    QStringList fileList = json.entryList(QDir::Files, QDir::Name);
    QString oldestFile = fileList.first();
    json.remove(oldestFile);
}

/**
 * creates for each log entry a json object and appends it to array
 * @param logList log entries from the mainwindow model
 * @param json resulting json Objects are placed in array
 */
void Log::writeJson(QStringList logList, QJsonArray &json){
    for(QString logEntry: logList){
        QStringList logColumns = logEntry.split(" ");
        QJsonObject logObject;
        logObject["time"] = logColumns.value(0);
        logObject["level"] = logColumns.value(1);
        logObject["source"] = logColumns.value(2);
        logObject["context"] = logColumns.value(3);
        logObject["payload"] = logColumns.value(4);
        json.append(logObject);
    }
}

void Log::readJson() {
    
}
