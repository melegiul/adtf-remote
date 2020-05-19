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
#include "networkclient.h"
#include "tmyadtfmessage.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), networkClient(new NetworkClient(this))
{
    ui->setupUi(this);

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openMapXML);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectNetwork);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectNetwork);

    connect(this->networkClient, &NetworkClient::connected, this, &MainWindow::networkConnected);
    connect(this->networkClient, &NetworkClient::disconnected, this, &MainWindow::networkDisconnected);
    connect(this->networkClient, &NetworkClient::received, this, &MainWindow::networkReceived);
    connect(this->networkClient, &NetworkClient::errored, this, &MainWindow::networkErrored);

    connect(ui->sb_counter, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::sendMyADTFMessage);
    // connect(ui->pb_send, &QPushButton::clicked, this, &MainWindow::send);
    // connect(this, &MainWindow::received, this, &MainWindow::msgHandlerDisplay);
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

void MainWindow::connectNetwork()
{
    QSettings settings;
    QString host = settings.value("preferences/ipaddress").toString();
    uint16_t port = settings.value("preferences/port").toUInt();

    this->networkClient->connectNetwork(host, port);
}

void MainWindow::disconnectNetwork()
{
    this->networkClient->disconnectNetwork();
}

void MainWindow::networkConnected() {
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    // TODO implement again
//    ui->statusbar->showMessage("Connected to " + settings.value("preferences/ipaddress").toString() + ":"
//                                + settings.value("preferences/port").toString());
}

void MainWindow::networkDisconnected() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
}

void MainWindow::networkReceived(ADTFMediaSample sample)
{
    tMyADTFMessage abc = tMyADTFMessage::fromNetwork(sample);
    ui->statusbar->showMessage(QString("Reveived %1 value %2").arg(sample.pinName.data()).arg(abc.sHeaderStruct.ui32HeaderVal));
}

void MainWindow::networkErrored(QString errorMsg) {
    ui->statusbar->showMessage(errorMsg);
}

// only for testing / demo
void MainWindow::sendMyADTFMessage(int counter) {
    tMyADTFMessage message {};
    message.sHeaderStruct.ui32HeaderVal = counter;
    message.sSimpleStruct.ui32Val = counter + 1;
    ADTFMediaSample sample = tMyADTFMessage::toNetwork(message, "counter", counter);
    this->networkClient->send(sample);

}
