#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>

#include "ui_dialog_log_analyzer.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    QString fileNameLog = nullptr;
    LogAnalyzerDialog(QWidget *parent = nullptr);
public slots:
    void loadLog();
    void saveLog();
};


#endif //DIALOG_LOG_ANALYZER_H
