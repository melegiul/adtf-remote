//
// Created by uniautonom on 15.08.20.
//

#include "log_model.h"

LogModel::LogModel(QObject *parent): QAbstractTableModel(parent) {}

/**
 * returns number of rows for children of parent
 * @param parent in most subclasses, number of rows is independent of parent
 * @return number of log entries
 */
int LogModel::rowCount(const QModelIndex &parent) const {
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(parent);
    return currentLog.size();
}

/**
 * returns number of columns for children of parent
 * @param parent in most subclasses, number of column is independent of parent
 * @return number of log columns
 */
int LogModel::columnCount(const QModelIndex &parent) const {
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(parent);
    return 5;
}

/**
 * retrieves a distinct value specified by the index (row, column)
 * @param index refers to item in table
 * @param role associated role for data elements of the items
 * @return QString, when value has DisplayRole (read only)
 */
QVariant LogModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();
    if(index.row() >= currentLog.size() || index.row() < 0)
        return QVariant();
    if (role == Qt::DisplayRole){
        QStringList logEntry = currentLog.value(index.row());
        return logEntry.value(index.column());
    }
    return QVariant();
}

/**
 * returns header data of the model
 * @param section for selecting the column (horizontal) and row number (vertical)
 * @param orientation can be horizontal or vertical
 * @param role associated role for data elements of the items, in this case DisplayRole (read only)
 * @return name of specified column header or index of row
 */
QVariant LogModel:: headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch(section) {
                case 0:
                    return QString("Time");
                case 1:
                    return QString("Level");
                case 2:
                    return QString("Source");
                case 3:
                    return QString("Context");
                case 4:
                    return QString("Payload");
            }
        }
        if (orientation == Qt::Vertical)
            return section+1;
    }
    return QVariant();
}

/**
 * inserts empty rows in the table
 * @param position index of position to insert the rows
 * if position is rowCount(), the rows are appended
 * @param count number of inserted rows
 * @param index specifies parent model index (unused)
 * @return true if successful
 */
bool LogModel::insertRows(int position, int count, const QModelIndex &index){
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(index);
    // ensures all connected views are aware of changes
    beginInsertRows(QModelIndex(), position, position + count - 1);
    for (int row=0; row < count; row++)
        currentLog.insert(position, {QString(), QString(), QString(), QString(), QString()});
    endInsertRows();
    return true;
}

/**
 * removes count number of rows at position
 * @param position start point for removing
 * @param count number of rows to remove
 * @param index parent model index
 * @return true if successful
 */
bool LogModel::removeRows(int position, int count, const QModelIndex &index){
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(index);
    // ensures all connected views are aware of changes
    beginRemoveRows(QModelIndex(), position, position + count - 1);
    for (int row=0; row < count; row++)
        currentLog.removeAt(position);
    endRemoveRows();
    return true;
}

/**
 * inserts data into the table, to fill a row, function must be called five times
 * for every column of the table
 * @param index contains row and column
 * @param value sets cell content to value
 * @param role associated role for data elements of the items
 * @return true if successful
 */
bool LogModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if (index.isValid() && role == Qt::DisplayRole){
        std::string str = value.toString().toStdString();
        int row = index.row();
        QStringList logEntry = currentLog.value(row);
        if (index.column() >= 0 && index.column() < 5)
            logEntry.insert(index.column(), value.toString());
        else
            return false;
        currentLog.replace(row, logEntry);
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

/**
 * retrieves data of the model
 * @return the current displayed log file
 */
QList<QStringList> &LogModel::getCurrentLog() {
    return currentLog;
}

/**
 * first retrieves settings for predefined memory location
 * then writes to json file and ensures the max file number limitation
 * @param logList list of current log entries in the mainwindow model
 */
void LogModel::saveLog(QList<QStringList> &logList, int maxFileNumber, QString fileName) {
    std::string  filePath;
    if (fileName.isEmpty()) {
        QSettings settings;
        QString qLogPath = settings.value("logPreferences/logPath", QDir::homePath()).toString();
        std::string logPath = qLogPath.toStdString();
        time_t now = time(0);
        char fileNameGen[40];
        tm *tm_info = localtime(&now);
        strftime(fileNameGen, 26, "/%Y-%m-%d-%H:%M:%S.json", tm_info);
        filePath = logPath + fileNameGen;
    } else {
        filePath = fileName.toStdString();
    }
    QFile saveFile(filePath.data());
    if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning("log_model.cpp-saveLog(): Could not open save file.");
        return;
    }
    LogSerialization::writeJson(logList, saveFile, maxFileNumber);
    saveFile.close();
}

/**
 * opens specified file and reads content
 * @param fileName absolute file name
 * @return entries of log file
 */
QList<QStringList> LogModel::loadLog(QString fileName){
    QFile jsonFile(fileName);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning("log_model.cpp-loadLog(): Could not open file");
    }
    QByteArray data = jsonFile.readAll();
    jsonFile.close();
    return LogSerialization::readJson(data);
}