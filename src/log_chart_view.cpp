//
// Created by uniautonom on 30.08.20.
//
#include <QtCharts/QChart>
#include "log_chart_view.h"
#include <QtDebug>

LogChartView::LogChartView(QWidget *parent = nullptr) {
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
    axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Occurrence");
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
    this->setRubberBand(QChartView::RectangleRubberBand);
}

void LogChartView::mouseReleaseEvent(QMouseEvent *e){
    if(e->button() == Qt::RightButton){
        chart->zoomReset();
        return;
    }
    QChartView::mouseReleaseEvent(e);
}


void LogChartView::setTick(  std::array<int, 6> &loglevelCount, int &yMax, int unit, int step){
    *set0 << loglevelCount[0];
    *set1 << loglevelCount[1];
    *set2 << loglevelCount[2];
    *set3 << loglevelCount[3];
    *set4 << loglevelCount[4];
    *set5 << loglevelCount[5];

    int msgCount = std::accumulate(loglevelCount.begin(), loglevelCount.end(),0);
    yMax = yMax>msgCount?yMax:msgCount;
    QDateTime time;
    time = unit == 0?QDateTime::fromMSecsSinceEpoch(step):QDateTime::fromSecsSinceEpoch(step);
    categories.append(time.toString(unit ==0?"mm:ss:zzz":unit==1?"mm:ss":"mm"));
}

void LogChartView::clearGraph(){

    set0 = new QBarSet("ACK");
    set1 = new QBarSet("CRITICAL");
    set2 = new QBarSet("ERROR");
    set3 = new QBarSet("WARNING");
    set4 = new QBarSet("INFO");
    set5 = new QBarSet("DEBUG");


    set0->setColor(QColor::fromRgb(35,103,151));
    set1->setColor(QColor::fromRgb(210,109,35));
    set2->setColor(QColor::fromRgb(151,35,35));
    set3->setColor(QColor::fromRgb(220,171,52));
    set4->setColor(QColor::fromRgb(95,165,52));
    set5->setColor(QColor::fromRgb(47,182,198));

    categories.clear();
    axisX->clear();
    series->clear();
    series->detachAxis(axisX);
    series->detachAxis(axisY);

    chart->zoomReset();
}

void LogChartView::fillGraph(int unit, int yMax) {
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);
    series->append(set5);

    axisX->append(categories);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    axisX->setTitleText(QString("t in %1").arg(unit ==0?"min:s:ms":unit==1?"min:s":"min"));
    axisY->setRange(0,yMax);

    QFont font;
    font.setPixelSize(11);
    axisX->setLabelsFont(font);

}
