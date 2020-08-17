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
    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
//    connect(this->clearLog,SIGNAL(clicked()),this,SLOT(on_clearButton_clicked()));
    //connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
    model = new LogModel(this);
//    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Time"));
//    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Level"));
//    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Source"));
//    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Context"));
//    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Payload"));
    model->headerData(0, Qt::Horizontal, Qt::DisplayRole);
    model->headerData(1, Qt::Horizontal, Qt::DisplayRole);
    model->headerData(2, Qt::Horizontal, Qt::DisplayRole);
    model->headerData(3, Qt::Horizontal, Qt::DisplayRole);
    model->headerData(4, Qt::Horizontal, Qt::DisplayRole);
    this->tableView->setModel(model);
//    this->tableView->horizontalHeader()
}

void LogAnalyzerDialog::loadLog() {
    //open dialog
//    fileNameLog = QFileDialog::getOpenFileName(this, "Choose LogSerialization file");
//    qDebug() << "LogSerialization file" << fileNameLog << "selected for opening";
//    if (fileNameLog.isNull()) return;
}

void LogAnalyzerDialog::saveLog() {

}

void LogAnalyzerDialog::handleLoadButtonClicked(){
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Json (*.json)"));
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        fileNames = dialog.selectedFiles();
    }
    QString string = fileNames.first();
//    model->populateData(log.loadLog(string));
    addEntries(log.loadLog(string));
    QStringList history = dialog.history();
    history.append(string);
    dialog.setHistory(history);
    this->historyBox->addItems(history);
}

void LogAnalyzerDialog::addEntries(QList<QStringList> logList) {
    for (QStringList logValues: logList){
        model->insertRows(0, 1, QModelIndex());
        for(int i=0; i<logValues.size(); i++){
            QModelIndex index = model->index(0,i,QModelIndex());
            model->setData(index, logValues.value(i), Qt::DisplayRole);
        }
    }
}

//void LogAnalyzerDialog::on_clearButton_clicked(){
//    this->tableWidget->setPlainText(nullptr);
//}