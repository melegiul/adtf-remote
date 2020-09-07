#include "proxy_model.h"
#include "qdebug.h"


ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {

    payloadFilter.setCaseSensitivity(Qt::CaseInsensitive);
    payloadFilter.setPatternSyntax(QRegExp::RegExp);

}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index1;
    QModelIndex index2;
    QModelIndex index3;
    QModelIndex index4;
    bool payload;

    index1 = sourceModel()->index(source_row, 1, source_parent);
    index2 = sourceModel()->index(source_row, 2, source_parent);
    index3 = sourceModel()->index(source_row, 3, source_parent);
    index4 = sourceModel()->index(source_row, 4, source_parent);

    //payloads from "ACK" Log Levels shall not be checked, because of no guarantee of it being a string
    if(payloadFilter.isEmpty())
        payload = true;
    else if (!(sourceModel()->data(index1).toString().contains("ACK", Qt::CaseInsensitive)))
        payload = sourceModel()->data(index4).toString().contains(payloadFilter);

    else
        payload = false;


    if (((logLevelFilter.contains(sourceModel()->data(index1).toString(), Qt::CaseInsensitive)) or
         logLevelFilter.isEmpty()) and
        ((sourceFilter.contains(sourceModel()->data(index2).toString(), Qt::CaseInsensitive)) or
         sourceFilter.isEmpty()) and
        ((contextFilter.contains(sourceModel()->data(index3).toString(), Qt::CaseInsensitive)) or
         contextFilter.isEmpty()) and
        payload)
        return true;
    return false;
}

void ProxyModel::setFilter(QStringList logLevelList, QStringList sourceList, QStringList contextList,
                           QString payloadString) {

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




