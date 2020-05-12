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

void MainWindow::connectToNetwork() {
    if (this->socket == nullptr) {
        this->socket = new QTcpSocket(this);
        in.setDevice(this->socket);
        connect(this->socket, &QIODevice::readyRead, this, &MainWindow::receive);
        connect(this->socket, QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::error), this, &MainWindow::displayError);
    }

    if (this->socket->state() != QTcpSocket::UnconnectedState) {
        qWarning() << "Activating connectToNetwork() slot shouldn't be possible as network is currently connected";
        return;
    }

    QSettings settings;
    this->socket->connectToHost(settings.value("preferences/ipaddress").toString(),
                                settings.value("preferences/port").toInt());

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->statusbar->showMessage("Connected to " + settings.value("preferences/ipaddress").toString() + ":"
                                + settings.value("preferences/port").toString());
}

void MainWindow::disconnectFromNetwork() {
    this->socket->disconnectFromHost();

    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->statusbar->showMessage("Disconnected");
}

void MainWindow::send() {
    QJsonObject jsonMain;
    jsonMain["type"] = "text";
    jsonMain["msg"] = ui->te_main->toPlainText();
    QJsonDocument jsonDoc(jsonMain);

    this->socket->write(jsonDoc.toJson(QJsonDocument::Compact));
    this->socket->write("\n");
    this->socket->flush();
}

void MainWindow::receive() {
//    this->uncompleteMessage.append(this->socket->readLine());
//    if (this->uncompleteMessage.isEmpty() || this->uncompleteMessage[this->uncompleteMessage.size() - 1] != '\n') return;

//    QJsonDocument jsonDoc = QJsonDocument::fromJson(this->uncompleteMessage);
//    if (jsonDoc.isNull())
//        qWarning() << "Message couldn't be parsed:" << this->uncompleteMessage;
//    this->uncompleteMessage.clear();
//    if (jsonDoc.isNull())
//        return;

//    qDebug() << "Received message:" << jsonDoc;
//    QJsonValue type = jsonDoc.object()["type"];
//    if (type.type() != QJsonValue::String) {
//        qWarning() << "Expected type in JSON object as string, got" << jsonDoc;
//        return;
//    }

//    qDebug() << "Message has type:" << type.toString();
//    emit received(type.toString(), jsonDoc.object());

    //QDataStream in(this->socket);
    //in.setVersion(QDataStream::Qt_4_7);

    char *pinName, *mediaType, *data;
    quint64 streamTime;
    uint length;

    in.startTransaction();

    in >> pinName >> mediaType >> streamTime;
    in.readBytes(data, length);
    qDebug() << "Message for pin" << pinName << "at time" << streamTime << "of length" << length << "with media type" << mediaType;
    delete [] pinName;
    delete [] mediaType;
    delete [] data;

    if (!in.commitTransaction()) {
        qDebug() << "Transaction commit failed";
        return;
    }
    qDebug() << "Transaction committed";
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
