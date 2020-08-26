#ifndef UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
#define UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel {
Q_OBJECT
public:
    ProxyModel(QObject *parent);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setFilter(QStringList logLevelList, QStringList sourceList, QStringList contextList);

private:
    QStringList logLevelFilter;
    QStringList sourceFilter;
    QStringList contextFilter;

};


#endif //UNIAUTONOM_REMOTEGUI_PROXY_MODEL_H
