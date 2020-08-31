//
// Created by uniautonom on 30.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
#define UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
#include <QtCharts>

using namespace QtCharts;

class LogChartView: public QChartView{
protected:
    void mouseReleaseEvent(QMouseEvent *e);
public:
    LogChartView(QWidget *parent);
    void setTick(  std::array<int, 6> &loglevelCount, int &yMax, int unit, int step);
    void clearGraph();
    void fillGraph(int unit, int yMax);

private:

    QStackedBarSeries *series;
    QStringList categories;
    QBarCategoryAxis *axisX;
    QValueAxis *axisY;
    QChart *chart;


    QBarSet *set0;
    QBarSet *set1;
    QBarSet *set2;
    QBarSet *set3;
    QBarSet *set4;
    QBarSet *set5;

};


#endif //UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
