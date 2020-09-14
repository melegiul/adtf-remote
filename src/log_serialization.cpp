//
// Created by uniautonom on 15.08.20.
//

#include "log_serialization.h"

LogSerialization::LogSerialization() {}

LogSerialization::LogSerialization(QDateTime minTime, QDateTime maxTime, QStringList logLevelFilter,
                                   QStringList sourceFilter,
                                   QStringList contextFilter, QRegExp payloadFilter) {
    this->minTime = minTime;
    this->maxTime = maxTime;
    this->logLevelFilter = logLevelFilter;
    this->sourceFilter = sourceFilter;
    this->contextFilter = contextFilter;
    this->payloadFilter = payloadFilter;
}


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
    QJsonObject wrapper, metadata;
    QJsonArray logData, logLevelArray, sourceArray, contextArray;
    QFileInfo fileInfo = QFileInfo(saveFile);
    QDir fileDir = fileInfo.absoluteDir();
    for (QStringList list : logVector) {
        QJsonObject logObject;
        logObject["time"] = list.value(0);
        logObject["level"] = list.value(1);
        logObject["source"] = list.value(2);
        logObject["context"] = list.value(3);
        logObject["payload"] = list.value(4);
        logData.append(logObject);
    }
    wrapper["logData"] = logData;
    metadata["minTime"] = minTime.toString(Qt::ISODate);
    metadata["maxTime"] = maxTime.toString(Qt::ISODate);
    for (QString logLevel: logLevelFilter)
        logLevelArray.append(QJsonValue(logLevel));
    metadata["logLevelFilter"] = logLevelArray;
    for (QString source: sourceFilter)
        sourceArray.append(QJsonValue(source));
    metadata["sourceFilter"] = sourceArray;
    for (QString context: contextFilter)
        contextArray.append(QJsonValue(context));
    metadata["contextFilter"] = contextArray;
    metadata["payloadFilter"] = payloadFilter.pattern();
    wrapper["filterData"] = metadata;
    saveFile.write(QJsonDocument(wrapper).toJson(QJsonDocument::Indented));
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
    QJsonObject wrapperObject;
    QJsonArray logData;
    if (!QJsonDocument::fromJson(data, &error).isNull()) {
        loadedDoc = QJsonDocument::fromJson(data);
    } else {
        std::string message = error.errorString().toStdString();
        throw std::runtime_error("Error parsing json file: " + message);
    }
    if (!loadedDoc.isObject()) {
        throw std::runtime_error("Json document contains an array, but an object was expected");
    }
    wrapperObject = loadedDoc.object();
    if (wrapperObject.contains("logData") && wrapperObject["logData"].isArray())
        logData = wrapperObject["logData"].toArray();
    else
        throw std::runtime_error("Json document does not contain log data array");
    QList<QStringList> logList = readLogData(logData);
    readFilterData(wrapperObject);
    int x = logList.size();
    return logList;
}

QList<QStringList> LogSerialization::readLogData(QJsonArray &logData) {
    QList<QStringList> logList;
    for (int i=0; i<logData.size(); i++) {
        QStringList logString;
        QJsonObject logEntry = logData[i].toObject();
        if (logEntry.contains("time") && logEntry["time"].isString())
            logString.append(logEntry["time"].toString());
        else
            throw std::runtime_error("Json log data object does not contain key time with string value");
        if (logEntry.contains("level") && logEntry["level"].isString())
            logString.append(logEntry["level"].toString());
        else
            throw std::runtime_error("Json log data object does not contain key level with string value");
        if (logEntry.contains("source") && logEntry["source"].isString())
            logString.append(logEntry["source"].toString());
        else
            throw std::runtime_error("Json log data object does not contain key source with string value");
        if (logEntry.contains("context") && logEntry["context"].isString())
            logString.append(logEntry["context"].toString());
        else
            throw std::runtime_error("Json log data object does not contain key context with string value");
        if (logEntry.contains("payload") && logEntry["payload"].isString())
            logString.append(logEntry["payload"].toString());
        else
            throw std::runtime_error("Json log data object does not contain key payload with string value");
        logList.prepend(logString);
    }
    int x = logList.size();
    return logList;
}

void LogSerialization::readFilterData(QJsonObject& wrapperObject){
    QJsonObject filterObject;
    if (wrapperObject.contains("filterData") && wrapperObject["filterData"].isObject())
        filterObject = wrapperObject["filterData"].toObject();
    else
        throw std::runtime_error("Json document does not contain filter data object");
    if (filterObject.contains("minTime") && filterObject["minTime"].isString())
        minTime = QDateTime::fromString(filterObject["minTime"].toString(), Qt::ISODate);
    else
        throw std::runtime_error("Json filter data object does not contain min time key");
    if (filterObject.contains("maxTime") && filterObject["maxTime"].isString())
        maxTime = QDateTime::fromString(filterObject["maxTime"].toString(), Qt::ISODate);
    else
        throw std::runtime_error("Json filter data object does not contain max time key");
    if (filterObject.contains("logLevelFilter") && filterObject["logLevelFilter"].isArray()) {
        QList<QVariant> logLevelList = filterObject["logLevelFilter"].toArray().toVariantList();
        for (QVariant logLevel: logLevelList)
            logLevelFilter.append(logLevel.toString());
    } else
        throw std::runtime_error("Json filter data object does not contain log level filter key");
    if (filterObject.contains("sourceFilter") && filterObject["sourceFilter"].isArray()) {
        QList<QVariant> sourceList = filterObject["sourceFilter"].toArray().toVariantList();
        for (QVariant source: sourceList)
            sourceFilter.append(source.toString());
    } else
        throw std::runtime_error("Json filter data object does not contain source filter key");
    if (filterObject.contains("contextFilter") && filterObject["contextFilter"].isArray()) {
        QList<QVariant> contextList = filterObject["contextFilter"].toArray().toVariantList();
        for (QVariant context: contextList)
            contextFilter.append(context.toString());
    } else
        throw std::runtime_error("Json filter data object does not contain context filter key");
    if (filterObject.contains("payloadFilter") && filterObject["payloadFilter"].isString())
        payloadFilter = QRegExp(filterObject["payloadFilter"].toString());
    else
        throw std::runtime_error("Json filter data object does not contain payload filter key");
}

const QDateTime &LogSerialization::getMinTime() const {
    return minTime;
}

const QDateTime &LogSerialization::getMaxTime() const {
    return maxTime;
}

const QStringList &LogSerialization::getLogLevelFilter() const {
    return logLevelFilter;
}

const QStringList &LogSerialization::getSourceFilter() const {
    return sourceFilter;
}

const QStringList &LogSerialization::getContextFilter() const {
    return contextFilter;
}

const QRegExp &LogSerialization::getPayloadFilter() const {
    return payloadFilter;
}
