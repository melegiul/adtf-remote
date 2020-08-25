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


    connect(this->loadButton,SIGNAL(clicked()), this, SLOT(updateGraph()));
    connect(this->directoryButton,SIGNAL(clicked()),this,SLOT(updateGraph()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateGraph()));
    connect(this->liveLogButton,SIGNAL(clicked()),this,SLOT(updateGraph()));
    connect(this->pushButton_4,SIGNAL(clicked()),this,SLOT(updateGraph()));

    connect(this->spinBox,SIGNAL(valueChanged(int)),this,SLOT(updateGraph()));
    connect(this->comboBox_3, SIGNAL(currentIndexChanged(int)),this, SLOT(updateGraph()));


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
    series->attachAxis(axisX);
    series->attachAxis(axisY);


    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);


//    QGraphicsScene *scene = new QGraphicsScene();
//    scene->addItem(chart);
//    this->graphicsView->setScene(scene);
//    this->graphicsView->viewport()->installEventFilter(this);


    graphicsView->setChart(chart);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setRubberBand(QChartView::RectangleRubberBand);

    updateGraph();

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


void LogAnalyzerDialog::updateGraph(){
    int numTotal = tableView->model()->rowCount();

    std::array<int, 6> loglevelCount = {0, 0, 0, 0, 0, 0};

    QStringList loglevel ={ "Ack" , "Critical" , "Error" , "Warning" , "Info" , "Debug"};

    clearGraph();

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
                setTick( loglevelCount, yMax, unit, step);
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
        setTick(  loglevelCount, yMax, unit, step);
    }
    fillGraph(unit, yMax);
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


void LogAnalyzerDialog::setTick(  std::array<int, 6> &loglevelCount, int &yMax, int unit, int step){
    *set0 << loglevelCount[0];
    *set1 << loglevelCount[1];
    *set2 << loglevelCount[2];
    *set3 << loglevelCount[3];
    *set4 << loglevelCount[4];
    *set5 << loglevelCount[5];

    int msgCount = std::accumulate(loglevelCount.begin(), loglevelCount.end(),0);
    yMax = yMax>msgCount?yMax:msgCount;
    categories.append(QVariant(unit !=2? step: ceil(step/60.0)).toString());
}

void LogAnalyzerDialog::clearGraph(){

    set0 = new QBarSet("ACK");
    set1 = new QBarSet("CRITICAL");
    set2 = new QBarSet("ERROR");
    set3 = new QBarSet("WARNING");
    set4 = new QBarSet("INFO");
    set5 = new QBarSet("DEBUG");


    set0->setColor(QColor::fromRgb(35,103,151));
    set1->setColor(QColor::fromRgb(151,35,35));
    set2->setColor(QColor::fromRgb(210,109,35));
    set3->setColor(QColor::fromRgb(220,171,52));
    set4->setColor(QColor::fromRgb(95,165,52));
    set5->setColor(QColor::fromRgb(47,182,198));

    categories.clear();
    axisX->clear();
    series->clear();
    series->detachAxis(axisX);
    series->detachAxis(axisY);
}

void LogAnalyzerDialog::fillGraph(int unit, int yMax){
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);
    series->append(set5);


    axisX->append(categories);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QString unitTxt = "ms";
    if(unit== 1){
        unitTxt = "s";
    }else if(unit ==2){
        unitTxt ="min";
    }
    axisX->setTitleText(QString("t in %1").arg(unitTxt));
    axisY->setRange(0,yMax);
    rect = chart->plotArea();
}

bool LogAnalyzerDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->graphicsView->viewport() && event->type() == QEvent::Resize){
        if(chart)
            chart->resize(this->graphicsView->viewport()->size());
    }
    return QDialog::eventFilter(watched, event);
}

void LogAnalyzerDialog::wheelEvent(QWheelEvent* event){
    this->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if(event->delta() >0){
        this->graphicsView->scale(scaleFactor, scaleFactor);

    }else {
        this->graphicsView->scale(1.0/scaleFactor, 1.0/scaleFactor);
    }
}
