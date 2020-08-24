#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>

#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
using namespace QtCharts;

#include "ui_dialog_log_analyzer.h"
#include "log_serialization.h"
#include "log_model.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer {
    Q_OBJECT

public:
    QString fileNameLog = nullptr;
    LogAnalyzerDialog(QWidget *parent = nullptr, LogModel* parentModel = nullptr);
//    void on_loadButton_clicked();
    bool eventFilter(QObject *watched, QEvent *event);

private:
    LogSerialization log;
    LogModel *model;
    LogModel *parentModel;
    QStringList *modelList = new QStringList();
    void addEntries(QList<QStringList> logList);
    void removeEntries();
    void get_timestep(int &timestep, int &unit_ind);

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

//    QFileDialog dialog;
public slots:
    void loadLog();
    void saveLog();

    void handleLoadButtonClicked();
//    void on_clearButton_clicked();
    void switchSource();
    void update_graph();
};


#endif //DIALOG_LOG_ANALYZER_H
