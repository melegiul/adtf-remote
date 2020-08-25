#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>
#include <QSortFilterProxyModel>

#include "ui_dialog_log_analyzer.h"
#include "log_serialization.h"
#include "log_model.h"
#include "proxy_model.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    QString fileNameLog = nullptr;
    LogAnalyzerDialog(QWidget *parent = nullptr, LogModel* parentModel = nullptr);
//    void on_loadButton_clicked();

private:
    LogSerialization log;
    LogModel *sourceModel;
    LogModel *parentModel;
    ProxyModel *proxyModel;
    QStringList *modelList = new QStringList();
    void addEntries(QList<QStringList> logList);
    void removeEntries();
    QStringList getFilterList(QListWidget* filterList);

//    QFileDialog dialog;
public slots:
    void loadLog();
    void saveLog();

    void handleLoadButtonClicked();
//    void on_clearButton_clicked();
    void handleApplyButtonClicked();
    void switchSource();

};


#endif //DIALOG_LOG_ANALYZER_H
