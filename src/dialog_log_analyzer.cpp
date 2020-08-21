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

#include <array>
#include <QtCore/QDateTime>
#include "dialog_log_analyzer.h"
#include "mainwindow.h"

LogAnalyzerDialog::LogAnalyzerDialog(QWidget *parent, LogModel *parentModel) : QDialog(parent), parentModel(parentModel) {
    this->setupUi(this);

//    QFileDialog dialog(this);
    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(handleLoadButtonClicked()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(switchSource()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(switchSource()));


    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(update_graph()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(update_graph()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(update_graph()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(update_graph()));
    connect(this->pushButton_4,SIGNAL(clicked()),this,SLOT(update_graph()));

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

    *set0 << 2;
    *set1 << 2;
    *set2 << 0;
    *set3 << 0;
    *set4 << 0;
    *set5 << 2;

    series = new QStackedBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);
    series->append(set5);

    chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::AllAnimations);


    categories<<"0.1";
    categories.append("Banane");
    axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    QChartView *chartView = new QChartView(chart);
    chartView->setParent(this->horizontalFrame);
    chartView->setRenderHint(QPainter::Antialiasing);

    update_graph();

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


void LogAnalyzerDialog::update_graph(){
    int num_total = tableView->model()->rowCount();
    if (num_total > 1) {
        std::array<int, 6> loglevel_count = {0, 0, 0, 0, 0, 0};

        QStringList loglevel;
        loglevel << "Ack" << "Critical" << "Error" << "Warning" << "Info" << "Debug";

        QBarSet *set0 = new QBarSet("ACK");
        QBarSet *set1 = new QBarSet("CRITICAL");
        QBarSet *set2 = new QBarSet("ERROR");
        QBarSet *set3 = new QBarSet("WARNING");
        QBarSet *set4 = new QBarSet("INFO");
        QBarSet *set5 = new QBarSet("DEBUG");

        qint64 timestep = 1; //in s
        QModelIndex ind_time = tableView->model()->index(0, 0, QModelIndex());
        QString text_time = tableView->model()->data(ind_time, Qt::DisplayRole).toString();
        QDateTime starttime = QDateTime::fromString(text_time, "dd.MM.yyyy hh:mm:ss:zzz");
        QDateTime time = QDateTime(starttime);
        time = time.addSecs(timestep);

        categories.clear();
        axis->clear();

        auto step = 0.0;

        for (int row = 0; row < num_total; ++row) {
            ind_time = tableView->model()->index(row, 0, QModelIndex());
            text_time = tableView->model()->data(ind_time, Qt::DisplayRole).toString();
            QModelIndex ind = tableView->model()->index(row, 1, QModelIndex());
            QString text = tableView->model()->data(ind, Qt::DisplayRole).toString();
            QDateTime new_time = QDateTime::fromString(text_time, "dd.MM.yyyy hh:mm:ss:zzz");

            if (new_time > time) {
                *set0 << loglevel_count[0];
                *set1 << loglevel_count[1];
                *set2 << loglevel_count[2];
                *set3 << loglevel_count[3];
                *set4 << loglevel_count[4];
                *set5 << loglevel_count[5];

                loglevel_count = {0, 0, 0, 0, 0, 0};
                categories.append(QVariant(step).toString());
                qDebug() << step;

                auto diff_in_ms = time.msecsTo(new_time);
                auto diff_in_steps = ceil(diff_in_ms/1000.0);
                time = time.addSecs(diff_in_steps);
                step += diff_in_steps;


            }
            if (!(tableView->isRowHidden(row))) {
                int loglevel_ind = loglevel.indexOf(text);
                if (loglevel_ind >= 0 && loglevel_ind < loglevel_count.size()) {
                    loglevel_count[loglevel_ind]++;
                }
            }
        }
        *set0 << loglevel_count[0];
        *set1 << loglevel_count[1];
        *set2 << loglevel_count[2];
        *set3 << loglevel_count[3];
        *set4 << loglevel_count[4];
        *set5 << loglevel_count[5];
        categories.append(QVariant(step).toString());


        series->clear();
        series->append(set0);
        series->append(set1);
        series->append(set2);
        series->append(set3);
        series->append(set4);
        series->append(set5);

        axis->append(categories);
        chart->removeAxis(axis);
        chart->createDefaultAxes();
        chart->setAxisX(axis, series);
        
    }
}


//void LogAnalyzerDialog::on_clearButton_clicked(){
//    this->tableWidget->setPlainText(nullptr);
//}