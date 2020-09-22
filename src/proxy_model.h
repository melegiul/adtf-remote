#ifndef UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
#define UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QDateTime>
#include "log_serialization.h"
#include "log_model.h"

class ProxyModel : public QSortFilterProxyModel {
Q_OBJECT
public:

    QString logName = NULL;

    ProxyModel(QObject *parent);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setFilter(QDateTime minTimeEntry, QDateTime maxTimeEntry, QStringList logLevelList, QStringList sourceList, QStringList contextList, QString payloadString);

    void setFilter(QStringList logLevelList);

    void saveLog(int maxFileNumber, QString defaultPath = QString(), QString fileName = QString(), int offset = 0);
    QList<QStringList> loadLog(QString fileName);

    const QDateTime &getMinTime() const;

    const QDateTime &getMaxTime() const;

    const QStringList &getLogLevelFilter() const;

    const QStringList &getSourceFilter() const;

    const QStringList &getContextFilter() const;

    const QRegExp &getPayloadFilter() const;

private:

    QDateTime minTime;
    QDateTime maxTime;
    QStringList logLevelFilter;
    QStringList sourceFilter;
    QStringList contextFilter;
    QRegExp payloadFilter;
};

#endif //UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
