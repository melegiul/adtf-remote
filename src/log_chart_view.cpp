//
// Created by uniautonom on 30.08.20.
//
#include <QtCharts/QChart>
#include "log_chart_view.h"
#include <QtDebug>
#include <QtSvg/QSvgGenerator>

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
    categories.append(QVariant(unit !=2? step: ceil(step/60.0)).toString());
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

    QString unitTxt = "ms";
    if(unit== 1){
        unitTxt = "s";
    }else if(unit ==2){
        unitTxt ="min";
    }
    axisX->setTitleText(QString("t in %1").arg(unitTxt));
    axisY->setRange(0,yMax);
}

void LogChartView::exportGraphAsSvg(QString fileName) {
    QString appDir = QDir::currentPath();
    if (fileName != NULL)
        appDir += "/../../export/" + fileName + ".svg";
    else
        appDir += "/../../export/log_graph.svg";

    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save SVG"),
                                                appDir, tr("SVG files (*.svg)"));

    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(chart->size().toSize());
    generator.setViewBox(chart->rect());

    QPainter painter;
    painter.begin(&generator);
    chart->scene()->render(&painter, chart->rect(), chart->rect());
    painter.end();
}
