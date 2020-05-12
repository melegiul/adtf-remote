#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QJsonObject>

class QTcpSocket;

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
    void connectToNetwork();
    void disconnectFromNetwork();
    void send();
    void receive();
    void msgHandlerDisplay(QString type, QJsonObject jsonObj);
    void displayError();

signals:
    void received(QString type, QJsonObject jsonObj);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket = nullptr;
    QByteArray uncompleteMessage;
    QDataStream in;
};
#endif // MAINWINDOW_H
