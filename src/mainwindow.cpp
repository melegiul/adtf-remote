#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTcpSocket>
#include <QtDebug>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_preferences.h"
#include "tmyadtfmessage.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openMapXML);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectToNetwork);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectFromNetwork);
    connect(ui->pb_send, &QPushButton::clicked, this, &MainWindow::send);
    connect(this, &MainWindow::received, this, &MainWindow::msgHandlerDisplay);

    in.setDevice(this->socket);
    in.setVersion(QDataStream::Qt_4_7);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openMapXML() {
    QString fileName = QFileDialog::getOpenFileName(this, "Choose map XML");
    qDebug() << "XML map file" << fileName << "selected for opening";
    if (fileName.isNull()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Cannot open file", "Cannot open file: " + file.errorString());
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->te_main->setPlainText(text);
    file.close();
}

void MainWindow::openPreferences() {
    PreferencesDialog prefDialog(this);
    prefDialog.exec();
}

void MainWindow::networkConnected() {
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->statusbar->showMessage("Connected to " + settings.value("preferences/ipaddress").toString() + ":"
                                + settings.value("preferences/port").toString());
}

void MainWindow::networkDisconnected() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->statusbar->showMessage("Disconnected");
}

void MainWindow::msgHandlerDisplay(QString type, QJsonObject jsonObj) {
    if (type != "display") return;

    if (jsonObj["msg"].type() != QJsonValue::String) {
        qWarning() << "Item msg not included in message type display or unexpected type";
        return;
    }

    ui->te_main->setPlainText(jsonObj["msg"].toString());
}

void MainWindow::displayError() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->statusbar->showMessage(QString("Disconnected due to error: %1").arg(this->socket->errorString()));
}

//QSettings settings;
//this->tcpSocket->connectToHost(settings.value("preferences/ipaddress").toString(),
//                            settings.value("preferences/port").toInt());
