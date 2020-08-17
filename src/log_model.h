//
// Created by uniautonom on 15.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOGMODEL_H
#define UNIAUTONOM_REMOTEGUI_LOGMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class LogModel : public QAbstractTableModel {
    Q_OBJECT
private:
    QList<QStringList> currentLog;
public:
    LogModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    void populateData(QStringList *logList);
};


#endif //UNIAUTONOM_REMOTEGUI_LOGMODEL_H
