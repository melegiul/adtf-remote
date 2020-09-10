#include "proxy_model.h"
#include "qdebug.h"


ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {

    payloadFilter.setCaseSensitivity(Qt::CaseInsensitive);
    payloadFilter.setPatternSyntax(QRegExp::RegExp);
}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index0;
    QModelIndex index1;
    QModelIndex index2;
    QModelIndex index3;
    QModelIndex index4;

    bool time, logLevel, source, context, payload;

    index0 = sourceModel()->index(source_row, 0, source_parent);
    index1 = sourceModel()->index(source_row, 1, source_parent);
    index2 = sourceModel()->index(source_row, 2, source_parent);
    index3 = sourceModel()->index(source_row, 3, source_parent);
    index4 = sourceModel()->index(source_row, 4, source_parent);


    //checking whether timeEntry is in filtered time range
    if (((QDateTime::fromString(sourceModel()->data(index0).toString(), "dd.MM.yyyy HH:mm:ss:zzz") < minTime) and
         minTime.isValid()) or
        ((QDateTime::fromString(sourceModel()->data(index0).toString(), "dd.MM.yyyy HH:mm:ss:zzz") > maxTime) and
         maxTime.isValid()))
        time = false;
    else
        time = true;

    //checking whether logLevelEntry shall be accepted or no Filter selected
    logLevel = ((logLevelFilter.contains(sourceModel()->data(index1).toString(), Qt::CaseInsensitive)) or
                logLevelFilter.isEmpty());
    //checking whether sourceEntry shall be accepted or no Filter selected
    source = ((sourceFilter.contains(sourceModel()->data(index2).toString(), Qt::CaseInsensitive)) or
              sourceFilter.isEmpty());

    //checking whether contextEntry shall be accepted or no Filter selected
    context = ((contextFilter.contains(sourceModel()->data(index3).toString(), Qt::CaseInsensitive)) or
               contextFilter.isEmpty());

    //checking whether payloadEntry shall be accepted by pattern or no Filter selected
    payload = sourceModel()->data(index4).toString().contains(payloadFilter) or payloadFilter.isEmpty();

    if (time and logLevel and source and context and payload)
        return true;
    return false;
}

void
ProxyModel::setFilter(QDateTime minTimeEntry, QDateTime maxTimeEntry, QStringList logLevelList, QStringList sourceList,
                      QStringList contextList,
                      QString payloadString) {
    minTime = minTimeEntry;
    maxTime = maxTimeEntry;
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





