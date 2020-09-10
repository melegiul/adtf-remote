#ifndef UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
#define UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QDateTime>

class ProxyModel : public QSortFilterProxyModel {
Q_OBJECT
public:
    ProxyModel(QObject *parent);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setFilter(QDateTime minTimeEntry, QDateTime maxTimeEntry, QStringList logLevelList, QStringList sourceList, QStringList contextList, QString payloadString);

    void setFilter(QStringList logLevelList);

private:

    QDateTime minTime;
    QDateTime maxTime;
    QStringList logLevelFilter;
    QStringList sourceFilter;
    QStringList contextFilter;
    QRegExp payloadFilter;
};

#endif //UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
