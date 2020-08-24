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
#include <QtCharts/QValueAxis>
#include <QGraphicsScene>
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
    connect(this->spinBox,SIGNAL(valueChanged(int)),this,SLOT(update_graph()));
    connect(this->comboBox_3, SIGNAL(currentIndexChanged(int)),this, SLOT(update_graph()));


    model = new LogModel(this);
    this->tableView->setModel(parentModel);
    this->tableView->setColumnWidth(0,160);
    this->tableView->setColumnWidth(1,40);
    this->tableView->setColumnWidth(2,120);
    this->tableView->setColumnWidth(3,80);
    this->tableView->horizontalHeader()->setStretchLastSection(true);

    series = new QStackedBarSeries();
    set0 = new QBarSet("ACK");
    set1 = new QBarSet("CRITICAL");
    set2 = new QBarSet("ERROR");
    set3 = new QBarSet("WARNING");
    set4 = new QBarSet("INFO");
    set5 = new QBarSet("DEBUG");

    chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);


    axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    axisX->setTitleText("t in ms");
    axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Occurrence");



    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);


    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(chart);
    this->graphicsView->setScene(scene);
    this->graphicsView->viewport()->installEventFilter(this);


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
    if (num_total > 0) {
        std::array<int, 6> loglevel_count = {0, 0, 0, 0, 0, 0};

        QStringList loglevel;
        loglevel << "Ack" << "Critical" << "Error" << "Warning" << "Info" << "Debug";

        set0 = new QBarSet("ACK");
        set1 = new QBarSet("CRITICAL");
        set2 = new QBarSet("ERROR");
        set3 = new QBarSet("WARNING");
        set4 = new QBarSet("INFO");
        set5 = new QBarSet("DEBUG");

        categories.clear();
        axisX->clear();

        int yMax = 0;
        int timestep;
        int unit;
        get_timestep(timestep,unit);
        int step = 0;
        int row = this->liveLogButton->isChecked()?num_total-1:0;
        QModelIndex ind_time = tableView->model()->index(row, 0, QModelIndex());
        QString text_time = tableView->model()->data(ind_time, Qt::DisplayRole).toString();
        QDateTime time = QDateTime::fromString(text_time, "dd.MM.yyyy hh:mm:ss:zzz");
        time = unit==0?time.addMSecs(timestep):time.addSecs(timestep);



        for (int r = 0; r < num_total; ++r) {
            row = this->liveLogButton->isChecked()?num_total-1-r:r;
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

                int tmp = std::accumulate(loglevel_count.begin(), loglevel_count.end(),0);
                yMax = yMax>tmp?yMax:tmp;
                loglevel_count = {0, 0, 0, 0, 0, 0};
                categories.append(QVariant(unit !=2?step:ceil(step/60.0)).toString());

                auto diff_in_ms = time.msecsTo(new_time);
                auto diff_in_steps = unit==0?diff_in_ms:ceil(diff_in_ms/1000.0);
                diff_in_steps = ceil(diff_in_steps/(1.0*timestep))*timestep;
                time = unit==0?time.addMSecs(diff_in_steps):time.addSecs(diff_in_steps);
                step += diff_in_steps;//unit==2?diff_in_steps/timestep*60:diff_in_steps/timestep;  // remove /timestep to only display unit not steps.


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
        int tmp = std::accumulate(loglevel_count.begin(), loglevel_count.end(),0);
        yMax = yMax>tmp?yMax:tmp;
        categories.append(QVariant(unit !=2?step:ceil(step/60.0)).toString());


        series->clear();
        series->append(set0);
        series->append(set1);
        series->append(set2);
        series->append(set3);
        series->append(set4);
        series->append(set5);

        chart->removeAxis(axisX);
        chart->removeAxis(axisY);
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        QString unit_txt = "ms";
        int time_step = timestep;
        if(unit== 1){
            unit_txt = "s";
        }else if(unit ==2){
            unit_txt ="min";
            time_step = timestep/60;
        }
        axisX->setTitleText(QString("t in %1").arg(unit_txt));
        // axisX->setTitleText(QString("t in %1 %2").arg(QVariant(time_step).toString(), unit_txt));
        axisY->setRange(0,yMax);
    }
}


void LogAnalyzerDialog::get_timestep(int &timestep, int &unit_ind){
    int step = this->spinBox->value();
    int unit =  1;

    unit_ind = this->comboBox_3->currentIndex();
    switch(unit_ind){
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
    timestep = step*unit;
}

bool LogAnalyzerDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->graphicsView->viewport() && event->type() == QEvent::Resize){
        if(chart)
            chart->resize(this->graphicsView->viewport()->size());
    }
    return QDialog::eventFilter(watched, event);
}