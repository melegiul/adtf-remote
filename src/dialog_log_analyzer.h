#ifndef DIALOG_LOG_ANALYZER_H
#define DIALOG_LOG_ANALYZER_H

#include <QDialog>
#include <QFileDialog>
#include <QStringListModel>
#include <QList>
#include <stdexcept>
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
    
    LogModel *sourceModel;
    LogModel *parentModel;
    ProxyModel *proxyModel;
    QStringList getFilterList(QListWidget* filterList);
    void resetFilterList(QListWidget* filterList);

    void setGuiFilter();
    void setGuiFilterList(QListWidget *filterList, QStringList currentProxyFilter);
    void resetGuiFilter();

    QDateTime getminDateTime();

    void getStepSize(int &stepSize, int &unit_ind);
    void getTimeAndText(int row, int numTotal, QDateTime &time, QString &text);
    void setTick(  std::array<int, 6> &loglevelCount, int &yMax, int unit, int step);
    void clearGraph();
    void fillGraph(int unit, int yMax);
    void getMedianAndMean(double &median, double &mean, int &min, int &max, std::vector <int> numbers);

public slots:
    void handleLoadButtonClicked();
    void handleSaveButtonClicked();
    void handleSaveAsButtonClicked();
    void switchSource();
    void saveSettings();
    void checkIndex();
    void updateGraph();
    void setFilter();
    void updateMetadata();
    void handleExportGraphClicked();
    void resetFilter();
    void handleHelpButtonClicked();
    void initTimeStamp();
};

#endif //DIALOG_LOG_ANALYZER_H
