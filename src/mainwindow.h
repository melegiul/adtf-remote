#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QJsonObject>

#include "adtfmediasample.h"

class NetworkClient;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openMapXML();
    void openPreferences();
    void connectNetwork();
    void disconnectNetwork();

    void networkConnected();
    void networkDisconnected();
    void networkReceived(ADTFMediaSample sample);
    void networkErrored(QString errorMsg);

    // testing / demo
    void sendMyADTFMessage(int counter);
private:
    Ui::MainWindow *ui;
    NetworkClient *networkClient;
};

#endif // __MAINWINDOW_H
