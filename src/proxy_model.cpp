#include "proxy_model.h"
#include "qdebug.h"


ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index1;
    QModelIndex index2;
    QModelIndex index3;

    index1 = sourceModel()->index(source_row, 1, source_parent);
    index2 = sourceModel()->index(source_row, 2, source_parent);
    index3 = sourceModel()->index(source_row, 3, source_parent);

    if (((logLevelFilter.contains(sourceModel()->data(index1).toString(), Qt::CaseInsensitive)) or
         logLevelFilter.isEmpty()) and
        ((sourceFilter.contains(sourceModel()->data(index2).toString(), Qt::CaseInsensitive)) or
         sourceFilter.isEmpty()) and
        ((contextFilter.contains(sourceModel()->data(index3).toString(), Qt::CaseInsensitive)) or
         contextFilter.isEmpty()))

        return true;
    return false;
}

void ProxyModel::setFilter(QStringList logLevelList, QStringList sourceList, QStringList contextList) {

    logLevelFilter = logLevelList;
    sourceFilter = sourceList;
    contextFilter = contextList;
    invalidateFilter();


}




