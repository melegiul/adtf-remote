//
// Created by uniautonom on 30.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
#define UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
#include <QtCharts/QChartView>

using namespace QtCharts;

class LogChartView: public QChartView{
protected:
    void mouseReleaseEvent(QMouseEvent *e);
public:
    void setDefaultRect(QRectF newRect);
    LogChartView(QWidget *parent);
};


#endif //UNIAUTONOM_REMOTEGUI_LOG_CHART_VIEW_H
