#ifndef LOG_ANALYSER_H
#define LOG_ANALYSER_H

#include <QDialog>
#include <QFileDialog>

#include "ui_log_analyser.h"

class LogAnalyser : public QDialog, public Ui_Loganalyser {
Q_OBJECT

private:
    QFileDialog dialog;
//    QString fileNameCarConfig = nullptr;
//    void showPreferences();
public:
    LogAnalyser(QWidget *parent = nullptr);
public slots:
    void on_loadButton_clicked();
    void on_clearButton_clicked();
//    void loadPreferences();
//    void savePreferences();
};

#endif // LOG_ANALYSER_H
