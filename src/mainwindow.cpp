#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_preferences.h"
#include "networkclient.h"
#include "adtf_structs/tmyadtfmessage.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), networkClient(new NetworkClient(this))
{
    ui->setupUi(this);

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionOpenMap, &QAction::triggered, this, &MainWindow::openMapXML);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectNetwork);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectNetwork);

    connect(this->networkClient, &NetworkClient::connected, this, &MainWindow::networkConnected);
    connect(this->networkClient, &NetworkClient::disconnected, this, &MainWindow::networkDisconnected);
    connect(this->networkClient, &NetworkClient::received, this, &MainWindow::networkReceived);
    connect(this->networkClient, &NetworkClient::errored, this, &MainWindow::networkErrored);

    connect(ui->sb_counter, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::sendMyADTFMessage);
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
    qDebug() << "XML map content:" << text;
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
    ui->statusbar->showMessage("Connected to " + this->networkClient->getPeer());
}

void MainWindow::networkDisconnected() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
}

void MainWindow::networkReceived(ADTFMediaSample sample)
{
    if (sample.pinName != "counter" || sample.mediaType != "tMyADTFMessage")
        return;

    tMyADTFMessage message = tMyADTFMessage::fromNetwork(sample);
    ui->lb_top->setText(QString("Reveived %1 value %2 on time %3").arg(sample.pinName.data()).arg(message.sHeaderStruct.ui32HeaderVal).arg(sample.streamTime));
}

void MainWindow::networkErrored(QString errorMsg) {
    ui->statusbar->showMessage(errorMsg);
}

// only for testing / demo
void MainWindow::sendMyADTFMessage(int counter) {
    tMyADTFMessage message {};
    message.sHeaderStruct.ui32HeaderVal = counter;
    message.sSimpleStruct.ui32Val = counter + 1;
    ADTFMediaSample sample = tMyADTFMessage::toNetwork(message, "counter", counter * 1000);
    this->networkClient->send(sample);

}
