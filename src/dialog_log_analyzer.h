#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>
#include <stdexcept>

#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>

using namespace QtCharts;
#include <QSortFilterProxyModel>


#include "ui_dialog_log_analyzer.h"
//#include "log_serialization.h"
#include "log_model.h"
#include "proxy_model.h"

class LogAnalyzerDialog : public QDialog, public Ui_dialog_log_analyzer{
    Q_OBJECT

public:
    LogAnalyzerDialog(QWidget *parent = nullptr, LogModel* parentModel = nullptr);
    void loadSettings();

private:
    QSettings settings;
    const QString fileDialogLabel = "File Dialog Selection";
    int maxFileNumber;
    void addEntries(QList<QStringList> logList);
    void clearModel();
    void updateFileHistory(QString fileName);

    LogSerialization log;
    LogModel *sourceModel;
    LogModel *parentModel;
    ProxyModel *proxyModel;
    QStringList *modelList = new QStringList();
    void removeEntries();
    QStringList getFilterList(QListWidget* filterList);


    void getStepSize(int &stepSize, int &unit_ind);
    void getTimeAndText(int row, int numTotal, QDateTime &time, QString &text);
    void setTick(  std::array<int, 6> &loglevelCount, int &yMax, int unit, int step);
    void clearGraph();
    void fillGraph(int unit, int yMax);

    QStackedBarSeries *series;
    QStringList categories;
    QBarCategoryAxis *axisX;
    QValueAxis *axisY;
    QChart *chart;
    QRectF rect;

    QBarSet *set0;
    QBarSet *set1;
    QBarSet *set2;
    QBarSet *set3;
    QBarSet *set4;
    QBarSet *set5;

public slots:
    void handleLoadButtonClicked();
    void handleSaveButtonClicked();
    void handleSaveAsButtonClicked();
    void switchSource();
    void saveSettings();
    void checkIndex();
    void updateGraph();
    void handleApplyButtonClicked();
};


#endif //DIALOG_LOG_ANALYZER_H
