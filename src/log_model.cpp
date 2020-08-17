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
 * @return QString, when value has DisplayRole
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
 * @param section for selecting the column
 * @param orientation can be horizontal or vertical
 * @param role associated role for data elements of the items
 * @return name of specified column header
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

QList<QStringList> &LogModel::getCurrentLog() {
    return currentLog;
}