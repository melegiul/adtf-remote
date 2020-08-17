//
// Created by uniautonom on 15.08.20.
//

#include "log_model.h"

LogModel::LogModel(QObject *parent): QAbstractTableModel(parent) {}

int LogModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return currentLog.size();
}

int LogModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 5;
}

QVariant LogModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();
    if(index.row() >= currentLog.size() || index.row() < 0)
        return QVariant();
    if (role == Qt::DisplayRole){
        QStringList logEntry = currentLog.value(index.row());

        int x = currentLog.size();
        std::string str;
        for (QStringList list: currentLog){
            int y = list.size();
            for (QString qstr: list) {
                str = qstr.toStdString();
            }
        }
        int qsize = logEntry.size();
        for (QString qstring: logEntry)
            str = qstring.toStdString();

        return logEntry.value(index.column());
    }
    return QVariant();
}

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
    }
    return QVariant();
}

void LogModel::populateData(QStringList *logList) {
//    currentLog = *logList;
//    for (int i=0; i<logList->size(); i++){
//        QString logEntry = logList->value(i);
//        QStringList logValues = logEntry.split(" ");
//        for (int j=0; j<logValues.size(); j++){
//            QModelIndex index = this->index(i,j,QModelIndex());
//            this->setData(index, logValues.value(j));
//        }
//    }
}

bool LogModel::insertRows(int position, int rows, const QModelIndex &index){
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(index);
    // ensures all connected views are aware of changes
    beginInsertRows(QModelIndex(), position, position + rows -1);
    for (int row=0; row<rows; row++)
        currentLog.insert(position, {QString(), QString(), QString(), QString(), QString()});
    endInsertRows();
    return true;
}
bool LogModel::removeRows(int position, int rows, const QModelIndex &index){
    // macro prevents compiler from warning about unused parameter
    Q_UNUSED(index);
    // ensures all connected views are aware of changes
    beginRemoveRows(QModelIndex(), position, position + rows -1);
    for (int row=0; row<rows; row++)
        currentLog.removeAt(position);
    endRemoveRows();
    return true;
}

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