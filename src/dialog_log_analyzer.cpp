#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <iostream>

#include <array>

#include "dialog_log_analyzer.h"
#include "mainwindow.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent, LogModel *parentModel) : QDialog(parent), parentModel(parentModel) {
    this->setupUi(this);

//    QFileDialog dialog(this);
    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(switchSource()));


    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(updateMetadata()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->pushButton_4,SIGNAL(clicked()),this,SLOT(updateMetadata()));
//    connect(this->clearLog,SIGNAL(clicked()),this,SLOT(on_clearButton_clicked()));
    //connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
    model = new LogModel(this);
    this->tableView->setModel(parentModel);
    this->tableView->setColumnWidth(0,160);
    this->tableView->setColumnWidth(1,40);
    this->tableView->setColumnWidth(2,120);
    this->tableView->setColumnWidth(3,80);
    this->tableView->horizontalHeader()->setStretchLastSection(true);

    updateMetadata();

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
    tableView->setModel(model);
    removeEntries();
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

void LogAnalyzerDialog::removeEntries() {
    model->removeRows(0, model->rowCount(), QModelIndex());
}

void LogAnalyzerDialog::switchSource() {
    if (this->liveLogButton->isChecked()){
        tableView->setModel(parentModel);
    } else {
        tableView->setModel(model);
    }
}

void LogAnalyzerDialog::updateMetadata(){
    int numTotal = tableView->model()->rowCount();
    std::array <int, 6> loglevelCount = {0,0,0,0,0,0};

    QStringList loglevel = { "Ack", "Critical" , "Error" , "Warning" , "Info" ,"Debug"};

    for (int row = 0; row < numTotal; ++row){
        QModelIndex ind = tableView->model()->index(row,1, QModelIndex());
        QString text = tableView->model()->data(ind,Qt::DisplayRole).toString();
           int  loglevelInd = loglevel.indexOf(text);
           if(loglevelInd >=0 && loglevelInd<loglevelCount.size()){
               loglevelCount[loglevelInd] ++;
        }
    }
    this->label_total->setText(QVariant(numTotal).toString());
    this->label_ack->setText(QVariant(loglevelCount[0]).toString());
    this->label_critical->setText(QVariant(loglevelCount[1]).toString());
    this->label_error->setText(QVariant(loglevelCount[2]).toString());
    this->label_warning->setText(QVariant(loglevelCount[3]).toString());
    this->label_info->setText(QVariant(loglevelCount[4]).toString());
    this->label_debug->setText(QVariant(loglevelCount[5]).toString());

}

//void LogAnalyzerDialog::on_clearButton_clicked(){
//    this->tableWidget->setPlainText(nullptr);
//}