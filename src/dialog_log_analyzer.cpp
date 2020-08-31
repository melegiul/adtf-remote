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
#include <QtCore/QDateTime>


#include "dialog_log_analyzer.h"
#include "qtooltipper.h"
#include "mainwindow.h"
#include "log_chart_view.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent, LogModel *parentModel) : QDialog(parent), parentModel(parentModel) {
    this->setupUi(this);

    connect(this->loadButton, SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
    connect(this->saveButton, SIGNAL(clicked()), this, SLOT(handleSaveButtonClicked()));
    connect(this->saveAsButton, SIGNAL(clicked()), this, SLOT(handleSaveAsButtonClicked()));
    connect(this->directoryButton, SIGNAL(clicked()), this, SLOT(switchSource()));
    connect(this->liveLogButton, SIGNAL(clicked()), this, SLOT(switchSource()));
    connect(this->historyBox, SIGNAL(activated(int)), this, SLOT(checkIndex()));
    connect(this, SIGNAL(rejected()), this, SLOT(saveSettings()));
    connect(this->applyButton, SIGNAL(clicked()), this, SLOT(handleApplyButtonClicked()));

    connect(this->loadButton, SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->directoryButton, SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->liveLogButton, SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->liveLogButton, SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->applyButton, SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->spinBox, SIGNAL(valueChanged(int)), this, SLOT(updateGraph()));
    connect(this->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGraph()));

    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(updateMetadata()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));
    connect(this->applyButton,SIGNAL(clicked()),this,SLOT(updateMetadata()));

    historyBox->setMaxCount(10);
    loadSettings();

    sourceModel = new LogModel(this);
    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(parentModel);
    proxyModel->setDynamicSortFilter(true);
    switchSource();

    this->tableView->setModel(proxyModel);
    this->tableView->setColumnWidth(0, 160);
    this->tableView->setColumnWidth(1, 40);
    this->tableView->setColumnWidth(2, 120);
    this->tableView->setColumnWidth(3, 80);


    this->tableView->horizontalHeader()->setStretchLastSection(true);
    this->tableView->viewport()->installEventFilter(new QToolTipper(this->tableView));

    updateGraph();
    updateMetadata();
}

/**
 * open customized file dialog for json file selection starting in specified serialization folder
 * if user selection is not empty, the file is loaded in the model and added to file selection history
 */
void LogAnalyzerDialog::handleLoadButtonClicked() {
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
            QMessageBox::warning(this, "File does not exist",
                                 historyBox->currentText() + " will now be removed from history");
            historyBox->removeItem(historyBox->currentIndex());
        }
    }
    if (!fileNames.isEmpty()) {
        // enters only if user made selection in file dialog and the file exists
        QString string = fileNames.first();
        clearModel();
        try {
            addEntries(((LogModel *) sourceModel)->loadLog(string));
            updateFileHistory(string);
        } catch (runtime_error &e) {
            QMessageBox::critical(this, "Json format error", e.what());
        }
    }
}

void LogAnalyzerDialog::handleApplyButtonClicked() {
    proxyModel->setFilter(getFilterList(logLevelListWidget), getFilterList(sourceListWidget),
                          getFilterList(contextListWidget), payloadLineEdit->text());

}

/**
 * history combo box is managed to show recent file calls on top,
 * while removing oldest entry, if max size exceeds
 * @param fileName most recent file call
 */
void LogAnalyzerDialog::updateFileHistory(QString fileName) {
    if (historyBox->findText(fileName) == -1) {
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
    historyBox->insertItem(0, fileName);
    // push file dialog label
    historyBox->insertItem(0, fileDialogLabel);
    historyBox->setCurrentText(fileName);
    loadButton->setDisabled(true);
    saveButton->setDisabled(false);
    saveAsButton->setDisabled(false);
}

/**
 * saves current model in file (selected by combo box)
 */
void LogAnalyzerDialog::handleSaveButtonClicked() {
    LogModel *currentModel = ((LogModel*)proxyModel);
    currentModel->saveLog(currentModel->getCurrentLog(), \
                            maxFileNumber,\
                            settings.value("logPreferences/logPath", "").toString(), \
                            historyBox->currentText());
}

/**
 * saves current model in new file selected by file dialog
 */
void LogAnalyzerDialog::handleSaveAsButtonClicked() {
    LogModel *currentModel = ((LogModel*)proxyModel);
    QString fileName = QFileDialog::getSaveFileName(this,\
                                tr("Save current log"),\
                                    settings.value("logPreferences/logPath", QDir::homePath()).toString(),\
                                    tr("Log File (*.json);;All files (*)"));

    if (!fileName.isEmpty()) {
        currentModel->saveLog(currentModel->getCurrentLog(), \
                            maxFileNumber, \
                            settings.value("logPreferences/logPath", "").toString(), \
                            fileName);
        updateFileHistory(fileName);
    }
}

/**
 * populates the model with the elements of logList
 * @param logList each qlist element represents a log entry containing time, source, context, etc.
 */
void LogAnalyzerDialog::addEntries(QList<QStringList> logList) {
    for (QStringList logValues: logList) {
        sourceModel->insertRows(0, 1, QModelIndex());
        for (int i = 0; i < logValues.size(); i++) {
            QModelIndex index = sourceModel->index(0, i, QModelIndex());
            sourceModel->setData(index, logValues.value(i), Qt::DisplayRole);
        }
    }
}


/**
 * removes the current displayed log file from the table view
 */
void LogAnalyzerDialog::clearModel() {
    sourceModel->removeRows(0, sourceModel->rowCount(), QModelIndex());
}

void LogAnalyzerDialog::removeEntries() {
    sourceModel->removeRows(0, sourceModel->rowCount(), QModelIndex());

}

/**
 * executes, when the state of radio buttons changed
 * switches parentModel (live log model) and model (logs loaded from files)
 */
void LogAnalyzerDialog::switchSource() {
    if (this->liveLogButton->isChecked()){
        proxyModel->setSourceModel(parentModel);
        historyBox->setDisabled(true);
        loadButton->setDisabled(true);
        saveButton->setDisabled(true);
        if (((LogModel*)parentModel)->getCurrentLog().isEmpty())
            saveAsButton->setDisabled(true);
        else
            saveAsButton->setDisabled(false);
    } else {
        proxyModel->setSourceModel(sourceModel);
        historyBox->setDisabled(false);
        checkIndex();
    }
}

/**
 * invoked, when file history combo box selection changes
 */
void LogAnalyzerDialog::checkIndex() {
    if (historyBox->currentText() == fileDialogLabel){
        // only loading possible, table view is blank
        loadButton->setDisabled(false);
        saveButton->setDisabled(true);
        saveAsButton->setDisabled(true);
        clearModel();
    } else {
        // loads automatic selected file and enables save buttons
        loadButton->setDisabled(true);
        saveButton->setDisabled(false);
        saveAsButton->setDisabled(false);
        handleLoadButtonClicked();
    }
}

/**
 * executes, when dialog quits
 * save the entries in combo box
 */
void LogAnalyzerDialog::saveSettings() {
    settings.setValue("logAnalyzer/liveButton", liveLogButton->isChecked());
    QStringList filePaths;
    historyBox->removeItem(historyBox->findText(fileDialogLabel));
    for(int i=0; i<historyBox->count(); i++)
        filePaths.append(historyBox->itemText(i));
    settings.setValue("logAnalyzer/historyFiles", filePaths);
}

/**
 * loads state of radio button and combo box entries, when log analyzer dialog starts
 */
void LogAnalyzerDialog::loadSettings(){
    liveLogButton->setChecked(settings.value("logAnalyzer/liveButton", true).toBool());
    QStringList pathList = settings.value("logAnalyzer/historyFiles").toStringList();
    historyBox->addItems(pathList);
    historyBox->insertItem(0,fileDialogLabel);
    maxFileNumber = settings.value("logPreferences/fileNumber",INT_MAX).toInt();
}

void LogAnalyzerDialog::updateGraph(){
    int numTotal = tableView->model()->rowCount();

    std::array<int, 6> loglevelCount = {0, 0, 0, 0, 0, 0};

    QStringList loglevel ={ "Ack" , "Critical" , "Error" , "Warning" , "Info" , "Debug"};

    graphicsView->clearGraph();

    int yMax = 0;
    int stepSize;
    int unit;
    getStepSize(stepSize,unit);
    int step = 0;

    if (numTotal > 0) {
        QString text;
        QDateTime time;
        getTimeAndText(0, numTotal, time, text);
        time = unit==0? time.addMSecs(stepSize): time.addSecs(stepSize);

        for (int row = 0; row < numTotal; ++row) {
            QDateTime newTime;
            getTimeAndText(row,numTotal, newTime, text);

            if (newTime > time) {
                graphicsView->setTick( loglevelCount, yMax, unit, step);
                loglevelCount = {0, 0, 0, 0, 0, 0};

                auto diffInMs = time.msecsTo(newTime);
                auto diffInSteps = unit==0? diffInMs: ceil(diffInMs/1000.0);
                diffInSteps = ceil(diffInSteps/(1.0*stepSize))*stepSize;
                time = unit==0? time.addMSecs(diffInSteps): time.addSecs(diffInSteps);
                step += diffInSteps;
            }
            int loglevelInd = loglevel.indexOf(text);
            if (loglevelInd >= 0 && loglevelInd < loglevelCount.size()) {
                loglevelCount[loglevelInd]++;
            }
        }
        graphicsView->setTick(  loglevelCount, yMax, unit, step);
    }
    graphicsView->fillGraph(unit, yMax);
}


void LogAnalyzerDialog::getStepSize(int &stepSize, int &unitInd){
    int step = this->spinBox->value();
    int unit =  1;

    unitInd = this->comboBox_3->currentIndex();
    switch(unitInd){
        case 0:     //ms in ms
            break;
        case 1:     //s in s
            unit = 1;
            break;
        case 2:     //min in s
            unit = 60;
            break;
        default:
            break;
    }
    stepSize = step*unit;
}

void LogAnalyzerDialog::getTimeAndText(int row, int numTotal, QDateTime &time, QString &text){
    int r = this->liveLogButton->isChecked()? numTotal-1-row: row;
    QModelIndex indTime = tableView->model()->index(r, 0, QModelIndex());
    QString textTime = tableView->model()->data(indTime, Qt::DisplayRole).toString();
    QModelIndex ind = tableView->model()->index(r, 1, QModelIndex());
    text = tableView->model()->data(ind, Qt::DisplayRole).toString();
    time = QDateTime::fromString(textTime, "dd.MM.yyyy hh:mm:ss:zzz");
}


QStringList LogAnalyzerDialog::getFilterList(QListWidget *filterList) {
    QStringList entries;
    for (int i = 0; i < filterList->count(); i++) {
        if (filterList->item(i)->checkState() == Qt::Checked) {
            entries.append(filterList->item(i)->text());
        }
    }
    return entries;
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
