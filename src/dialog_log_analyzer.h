#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>
#include <stdexcept>

#include "ui_dialog_log_analyzer.h"
//#include "log_serialization.h"
#include "log_model.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    LogAnalyzerDialog(QWidget *parent = nullptr, QAbstractTableModel* parentModel = nullptr);
    void loadSettings();

private:
    QAbstractTableModel *model;
    QAbstractTableModel *parentModel;
    QSettings settings;
    const QString fileDialogLabel = "File Dialog Selection";
    int maxFileNumber;
    void addEntries(QList<QStringList> logList);
    void clearModel();
    void updateFileHistory(QString fileName);

public slots:
    void handleLoadButtonClicked();
    void handleSaveButtonClicked();
    void handleSaveAsButtonClicked();
    void switchSource();
    void saveSettings();
    void checkIndex();
};


#endif //DIALOG_LOG_ANALYZER_H
