#include "proxy_model.h"
#include "qdebug.h"


ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {

    payloadFilter.setCaseSensitivity(Qt::CaseInsensitive);
    payloadFilter.setPatternSyntax(QRegExp::RegExp);
}
/**
* function to check whether a row of the data shall be shown
 * returns true if row is accepted and false if not
 *
*/
bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index0;
    QModelIndex index1;
    QModelIndex index2;
    QModelIndex index3;
    QModelIndex index4;

    bool time, logLevel, source, context, payload;

    index0 = sourceModel()->index(source_row, 0, source_parent);
    index1 = sourceModel()->index(source_row, 1, source_parent);
    index2 = sourceModel()->index(source_row, 2, source_parent);
    index3 = sourceModel()->index(source_row, 3, source_parent);
    index4 = sourceModel()->index(source_row, 4, source_parent);


    //checking whether timeEntry is in filtered time range
    if (((QDateTime::fromString(sourceModel()->data(index0).toString(), "dd.MM.yyyy HH:mm:ss:zzz") < minTime) and
         minTime.isValid()) or
        ((QDateTime::fromString(sourceModel()->data(index0).toString(), "dd.MM.yyyy HH:mm:ss:zzz") > maxTime) and
         maxTime.isValid()))
        time = false;
    else
        time = true;

    //checking whether logLevelEntry shall be accepted or no Filter selected
    logLevel = ((logLevelFilter.contains(sourceModel()->data(index1).toString(), Qt::CaseInsensitive)) or
                logLevelFilter.isEmpty());
    //checking whether sourceEntry shall be accepted or no Filter selected
    source = ((sourceFilter.contains(sourceModel()->data(index2).toString(), Qt::CaseInsensitive)) or
              sourceFilter.isEmpty());

    //checking whether contextEntry shall be accepted or no Filter selected
    context = ((contextFilter.contains(sourceModel()->data(index3).toString(), Qt::CaseInsensitive)) or
               contextFilter.isEmpty());

    //checking whether payloadEntry shall be accepted by pattern or no Filter selected
    payload = sourceModel()->data(index4).toString().contains(payloadFilter) or payloadFilter.isEmpty();

    if (time and logLevel and source and context and payload)
        return true;
    return false;
}
/**
* updates all filter by which the entries will be filtered
*/
void
ProxyModel::setFilter(QDateTime minTimeEntry, QDateTime maxTimeEntry, QStringList logLevelList, QStringList sourceList,
                      QStringList contextList,
                      QString payloadString) {
    minTime = minTimeEntry;
    maxTime = maxTimeEntry;
    logLevelFilter = logLevelList;
    sourceFilter = sourceList;
    contextFilter = contextList;
    payloadFilter.setPattern(payloadString);
    invalidateFilter();
}

void ProxyModel::setFilter(QStringList logLevelList) {
    logLevelFilter = logLevelList;
    invalidateFilter();
}

/**
 * first retrieves settings for predefined memory location
 * then writes to json file and ensures the max file number limitation
 * @param logList list of current log entries in the model
 */
void ProxyModel::saveLog(int maxFileNumber, QString defaultPath, QString fileName, int offset) {
    QVector<QStringList> logVector = ((LogModel*)this->sourceModel())->getCurrentLog().toVector();
    logVector.remove(logVector.size()-offset, offset);
    if (logVector.isEmpty())
        return;
    std::string  filePath;
    if (fileName.isEmpty()) {
        // main window: automatic save calls function without file name
        QDir dir = QDir(defaultPath);
        if (!dir.exists()) {
            // creates default directory if folder does not exist
            dir.cdUp();
            dir.mkdir("json");
        }
        std::string logPath = defaultPath.toStdString();
        time_t now = time(0);
        char fileNameGen[40];
        tm *tm_info = localtime(&now);
        // sets auto generated file name to current date
        strftime(fileNameGen, 26, "/%Y-%m-%d-%H:%M:%S.json", tm_info);
        filePath = logPath + fileNameGen;
        logName = fileNameGen;
        logName = logName.remove(".json");

    } else {
        // save buttons in log analyzer dialog retrieve a user defined file name containing the absolute path
        filePath = fileName.toStdString();
    }
    QFile saveFile(filePath.data());
    if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning("proxy_model.cpp-saveLog(): Could not open save file.");
        return;
    }
    LogSerialization objectToJson(this->minTime,
            this->maxTime,
            this->logLevelFilter,
            this->sourceFilter,
            this->contextFilter,
            this->payloadFilter);
    objectToJson.writeJson(logVector, saveFile, maxFileNumber);
    saveFile.close();
}

/**
 * opens specified file, reads content and sets the corresponding proxy model attributes
 * @param fileName absolute file name
 * @return entries of log file
 */
QList<QStringList> ProxyModel::loadLog(QString fileName) {
    QList<QStringList> logData;
    QFile jsonFile(fileName);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        throw std::runtime_error("proxy_model.cpp-loadLog(): Could not open file");
    }
    QByteArray data = jsonFile.readAll();
    jsonFile.close();
    LogSerialization jsonToObject;
    logData = jsonToObject.readJson(data);
    this->setFilter(jsonToObject.getMinTime(),
            jsonToObject.getMaxTime(),
            jsonToObject.getLogLevelFilter(),
            jsonToObject.getSourceFilter(),
            jsonToObject.getContextFilter(),
            jsonToObject.getPayloadFilter().pattern());
    return logData;
}

const QDateTime &ProxyModel::getMinTime() const {
    return minTime;
}

const QDateTime &ProxyModel::getMaxTime() const {
    return maxTime;
}

const QStringList &ProxyModel::getLogLevelFilter() const {
    return logLevelFilter;
}

const QStringList &ProxyModel::getSourceFilter() const {
    return sourceFilter;
}

const QStringList &ProxyModel::getContextFilter() const {
    return contextFilter;
}

const QRegExp &ProxyModel::getPayloadFilter() const {
    return payloadFilter;
}





