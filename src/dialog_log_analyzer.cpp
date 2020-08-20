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
    connect(this->saveButton,SIGNAL(clicked()),this,SLOT(handleSaveButtonClicked()));
    connect(this->saveAsButton,SIGNAL(clicked()),this,SLOT(handleSaveAsButtonClicked()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->historyBox,SIGNAL(activated(int)),this,SLOT(checkIndex()));
    connect(this,SIGNAL(rejected()),this,SLOT(saveSettings()));
    historyBox->setMaxCount(10);
    loadSettings();
    model = new LogModel(this);
    switchSource();
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
    if (historyBox->currentText() == fileDialogLabel) {
        // log file selection from file dialog
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(tr("Json (*.json)"));
        dialog.setDirectory(QDir(settings.value("logPreferences/logPath", QDir::homePath()).toString()));
        if (dialog.exec()) {
            fileNames = dialog.selectedFiles();
        }
    } else {
        // log file selection from history combo box
        if (QFile::exists(historyBox->currentText())) {
            // if selected file exists, it will be appended
            fileNames.append(historyBox->currentText());
        } else {
            // fileNames remains empty
            QMessageBox::warning(this, "File does not exist", historyBox->currentText() + " will now be removed from history");
            historyBox->removeItem(historyBox->currentIndex());
        }
    }
    if (!fileNames.isEmpty()) {
        // enters only if user made selection in file dialog and the file exists
        QString string = fileNames.first();
        clearModel();
        addEntries(((LogModel*)model)->loadLog(string));
        historyBox->setCurrentText(string);
        if (historyBox->findText(string) == -1) {
            // if the loaded file does not exist in combo box, insert new entry
            if (historyBox->count() >= historyBox->maxCount())
                // if the maximum number of combobox entries is reached, remove oldest entry
                historyBox->removeItem(historyBox->count() - 1);
        } else {
            // if loaded file already exists, update call order
            historyBox->removeItem(historyBox->currentIndex());
        }
        // pop file dialog label
        historyBox->removeItem(0);
        // push recent file
        historyBox->insertItem(0, string);
        // push file dialog label
        historyBox->insertItem(0, fileDialogLabel);
        historyBox->setCurrentText(string);
        loadButton->setDisabled(true);
        saveButton->setDisabled(false);
        saveAsButton->setDisabled(false);
    }
}

void LogAnalyzerDialog::handleSaveButtonClicked() {
    LogModel *currentModel = ((LogModel*)model);
    currentModel->saveLog(currentModel->getCurrentLog(), maxFileNumber, historyBox->currentText());
}

void LogAnalyzerDialog::handleSaveAsButtonClicked() {
//    QStringList fileNames;
    LogModel *currentModel = ((LogModel*)model);
    QString fileName = QFileDialog::getSaveFileName(this,\
                                tr("Save current log"),\
                                    settings.value("logPreferences/logPath", QDir::homePath()).toString(),\
                                    tr("Log File (*.json);;All files (*)"));
    /*QFileDialog saveAsDialog(this);
    saveAsDialog.setFileMode(QFileDialog::AnyFile);
    saveAsDialog.setNameFilter(tr("Json (*.json)"));
    saveAsDialog.setDirectory(QDir(settings.value("logPreferences/logPath", QDir::homePath()).toString()));
    if (saveAsDialog.exec()) {
        fileNames = saveAsDialog.selectedFiles();
    }*/
    if (!fileName.isEmpty())
        currentModel->saveLog(currentModel->getCurrentLog(), maxFileNumber, fileName);
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
void LogAnalyzerDialog::clearModel() {
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
        saveButton->setDisabled(true);
        if (((LogModel*)parentModel)->getCurrentLog().isEmpty())
            saveAsButton->setDisabled(true);
        else
            saveAsButton->setDisabled(false);
    } else {
        tableView->setModel(model);
        historyBox->setDisabled(false);
        checkIndex();
    }
}

void LogAnalyzerDialog::checkIndex() {
    if (historyBox->currentText() == fileDialogLabel){
        loadButton->setDisabled(false);
        saveButton->setDisabled(true);
        saveAsButton->setDisabled(true);
        clearModel();
    } else {
        loadButton->setDisabled(true);
        saveButton->setDisabled(false);
        saveAsButton->setDisabled(false);
        handleLoadButtonClicked();
    }
}

//void LogAnalyzerDialog::clearModel(){
//    LogModel* currentModel = ((LogModel*)model);
//    currentModel->removeRows(0,currentModel->rowCount());
//}

void LogAnalyzerDialog::saveSettings() {
    settings.setValue("logAnalyzer/liveButton", liveLogButton->isChecked());
    QStringList filePaths;
    historyBox->removeItem(historyBox->findText(fileDialogLabel));
    for(int i=0; i<historyBox->count(); i++)
        filePaths.append(historyBox->itemText(i));
    settings.setValue("logAnalyzer/historyFiles", filePaths);
}

void LogAnalyzerDialog::loadSettings(){
    liveLogButton->setChecked(settings.value("logAnalyzer/liveButton", true).toBool());
    QStringList pathList = settings.value("logAnalyzer/historyFiles").toStringList();
    historyBox->addItems(pathList);
    historyBox->insertItem(0,fileDialogLabel);
    maxFileNumber = settings.value("logPreferences/fileNumber",INT_MAX).toInt();
}
