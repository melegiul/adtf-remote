//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOGMODEL_H
#define UNIAUTONOM_REMOTEGUI_LOGMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include <QTextStream>
#include <stdexcept>
#include "log_serialization.h"

/**
 * standard interface for models with two dimensional array of items
 */
class LogModel : public QAbstractTableModel {
    Q_OBJECT
private:
    QList<QStringList> currentLog;
public:
    LogModel(QObject *parent);
    QList<QStringList> &getCurrentLog();
    void saveLog(QList<QStringList> &logList, int maxFileNumber, QString defaultPath, QString fileName = QString(), int offset = 0);
    QList<QStringList> loadLog(QString fileName);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole)override;
    bool insertRows(int position, int count, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int count, const QModelIndex &index = QModelIndex()) override;
};


#endif //UNIAUTONOM_REMOTEGUI_LOGMODEL_H
