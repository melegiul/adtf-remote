#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>

#include "ui_dialog_log_analyzer.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    QString fileNameLog = nullptr;
    LogAnalyzerDialog(QWidget *parent = nullptr);

//    QFileDialog dialog;
public slots:
    void loadLog();
    void saveLog();

    void on_loadButton_clicked();
//    void on_clearButton_clicked();
};


#endif //DIALOG_LOG_ANALYZER_H
