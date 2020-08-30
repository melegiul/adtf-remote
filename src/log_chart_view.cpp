//
// Created by uniautonom on 30.08.20.
//
#include <QtCharts/QChart>
#include "log_chart_view.h"
#include <QtDebug>

LogChartView::LogChartView(QWidget *parent = nullptr) {

}

void LogChartView::mouseReleaseEvent(QMouseEvent *e){
    if(e->button() == Qt::RightButton){
        this->chart()->zoomReset();
        return;
    }
    QChartView::mouseReleaseEvent(e);
}