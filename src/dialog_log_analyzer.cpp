#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include "dialog_log_analyzer.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent) : QDialog(parent) {
    this->setupUi(this);

    //connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
}

void LogAnalyzerDialog::loadLog() {
    //open dialog
    fileNameLog = QFileDialog::getOpenFileName(this, "Choose Log file");
    qDebug() << "Log file" << fileNameLog << "selected for opening";
    if (fileNameLog.isNull()) return;
}

void LogAnalyzerDialog::saveLog() {

}