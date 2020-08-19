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
#include "mainwindow.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent, QAbstractTableModel *parentModel) : QDialog(parent), parentModel(parentModel) {
    this->setupUi(this);
    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this,SIGNAL(rejected()),this,SLOT(saveSettings()));
    historyBox->setMaxCount(10);
    loadSettings();
    model = new LogModel(this);
    if (!this->directoryButton->isChecked()){
        this->tableView->setModel(parentModel);
        this->historyBox->setDisabled(true);
        this->loadButton->setDisabled(true);
    } else {
        this->tableView->setModel(model);
        this->historyBox->setDisabled(false);
        this->loadButton->setDisabled(false);
    }
    this->tableView->setColumnWidth(0,180);
    this->tableView->setColumnWidth(1,40);
    this->tableView->setColumnWidth(2,120);
    this->tableView->setColumnWidth(3,100);
    this->tableView->horizontalHeader()->setStretchLastSection(true);
}

/**
 * open customized file dialog for json file selection starting in specified serialization folder
 * if user selection is not empty, the file is loaded in the model and added to file selection history
 */
void LogAnalyzerDialog::handleLoadButtonClicked(){
    QStringList fileNames;
    if (historyBox->currentText() == "File Dialog Selection") {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(tr("Json (*.json)"));
        dialog.setDirectory(QDir(settings.value("logPreferences/logPath", QDir::homePath()).toString()));
        if (dialog.exec()) {
            fileNames = dialog.selectedFiles();
        }
    } else {
        if (QFile::exists(historyBox->currentText())) {
            fileNames.append(historyBox->currentText());
        } else {
            QMessageBox::warning(this, "File does not exist", historyBox->currentText() + " will now be removed from history");
            historyBox->removeItem(historyBox->currentIndex());
        }
    }
    if (!fileNames.isEmpty()) {
        QString string = fileNames.first();
        removeEntries();
        addEntries(((LogModel*)model)->loadLog(string));
        if (historyBox->findText(string) == -1) {
            if (historyBox->count() >= historyBox->maxCount())
                historyBox->removeItem(historyBox->count() - 1);
            historyBox->removeItem(0);
            historyBox->insertItem(0, string);
            historyBox->insertItem(0, "File Dialog Selection");
        }
    }
}

/**
 * populates the model with the elements of logList
 * @param logList each qlist element represents a log entry containing time, source, context, etc.
 */
void LogAnalyzerDialog::addEntries(QList<QStringList> logList) {
    for (QStringList logValues: logList){
        model->insertRows(0, 1, QModelIndex());
        for(int i=0; i<logValues.size(); i++){
            QModelIndex index = model->index(0,i,QModelIndex());
            model->setData(index, logValues.value(i), Qt::DisplayRole);
        }
    }
}

/**
 * removes the current displayed log file from the table view
 */
void LogAnalyzerDialog::removeEntries() {
    model->removeRows(0, model->rowCount(), QModelIndex());
}

/**
 * executes, when the state of radio buttons changed
 * switches parentModel (live log model) and model (logs loaded from files)
 */
void LogAnalyzerDialog::switchSource() {
    if (this->liveLogButton->isChecked()){
        tableView->setModel(parentModel);
        historyBox->setDisabled(true);
        loadButton->setDisabled(true);
    } else {
        tableView->setModel(model);
        historyBox->setDisabled(false);
        loadButton->setDisabled(false);
    }
}

void LogAnalyzerDialog::saveSettings() {
    settings.setValue("logAnalyzer/liveButton", liveLogButton->isChecked());
    QStringList filePaths;
    historyBox->removeItem(historyBox->findText("File Dialog Selection"));
    for(int i=0; i<historyBox->count(); i++)
        filePaths.append(historyBox->itemText(i));
    settings.setValue("logAnalyzer/historyFiles", filePaths);
}

void LogAnalyzerDialog::loadSettings(){
    liveLogButton->setChecked(settings.value("logAnalyzer/liveButton", true).toBool());
    QStringList pathList = settings.value("logAnalyzer/historyFiles").toStringList();
    historyBox->addItems(pathList);
    historyBox->insertItem(0,"File Dialog Selection");
}
