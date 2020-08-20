#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <iostream>

#include <QtCharts/QChartView>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
using namespace QtCharts;
//QT_CHARTS_USE_NAMESPACE
#include "dialog_log_analyzer.h"
#include "mainwindow.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent, LogModel *parentModel) : QDialog(parent), parentModel(parentModel) {
    this->setupUi(this);

//    QFileDialog dialog(this);
    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(switchSource()));
//    connect(this->clearLog,SIGNAL(clicked()),this,SLOT(on_clearButton_clicked()));
    //connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
    model = new LogModel(this);
    this->tableView->setModel(parentModel);
    this->tableView->setColumnWidth(0,160);
    this->tableView->setColumnWidth(1,40);
    this->tableView->setColumnWidth(2,120);
    this->tableView->setColumnWidth(3,80);
    this->tableView->horizontalHeader()->setStretchLastSection(true);


    QBarSet *set0 = new QBarSet("ACK");
    QBarSet *set1 = new QBarSet("CRITICAL");
    QBarSet *set2 = new QBarSet("ERROR");
    QBarSet *set3 = new QBarSet("WARNING");
    QBarSet *set4 = new QBarSet("INFO");
    QBarSet *set5 = new QBarSet("DEBUG");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
    *set4 << 9 << 7 << 5 << 3 << 1 << 2;
    *set5 << 9 << 7 << 5 << 3 << 1 << 2;

    QStackedBarSeries *series = new QStackedBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);
    series->append(set5);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "0.1" << "0.2" << "0.3" << "0.4" << "0.5" << "0.6";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    QChartView *chartView = new QChartView(chart);
    chartView->setParent(this->horizontalFrame);
    chartView->setMinimumSize(600,300);
    chartView->setRenderHint(QPainter::Antialiasing);
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

//void LogAnalyzerDialog::on_clearButton_clicked(){
//    this->tableWidget->setPlainText(nullptr);
//}