#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <iostream>

#include "dialog_log_analyzer.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent) : QDialog(parent) {
    this->setupUi(this);

//    QFileDialog dialog(this);
//    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(on_loadButton_clicked()));
//    connect(this->clearLog,SIGNAL(clicked()),this,SLOT(on_clearButton_clicked()));
    //connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
}

void LogAnalyzerDialog::loadLog() {
    //open dialog
//    fileNameLog = QFileDialog::getOpenFileName(this, "Choose Log file");
//    qDebug() << "Log file" << fileNameLog << "selected for opening";
//    if (fileNameLog.isNull()) return;
}

void LogAnalyzerDialog::saveLog() {

}

void LogAnalyzerDialog::on_loadButton_clicked(){
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Json (*.json)"));
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        fileNames = dialog.selectedFiles();
    }
//    QString file_name = QFileDialog::getOpenFileName(this, "Select log file", QDir::homePath());
    QString string = fileNames.first();
    QFile file(string);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Could not open file");
    }
    QTextStream in(&file);
    QString text = in.readAll();
//    this->tableWidget->
    QStringList history = dialog.history();
    history.append(string);
    dialog.setHistory(history);
    this->historyBox->addItems(history);
    file.close();
}

//void LogAnalyzerDialog::on_clearButton_clicked(){
//    this->tableWidget->setPlainText(nullptr);
//}