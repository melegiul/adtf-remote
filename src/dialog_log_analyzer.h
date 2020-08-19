#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>

#include "ui_dialog_log_analyzer.h"
#include "log_serialization.h"
#include "log_model.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    LogAnalyzerDialog(QWidget *parent = nullptr, QAbstractTableModel* parentModel = nullptr);
    void loadSettings();

private:
    LogSerialization log;
    QAbstractTableModel *model;
    QAbstractTableModel *parentModel;
    QSettings settings;
    void addEntries(QList<QStringList> logList);
    void removeEntries();

public slots:
    void handleLoadButtonClicked();
    void switchSource();
    void saveSettings();
};


#endif //DIALOG_LOG_ANALYZER_H
