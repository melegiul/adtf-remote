//
// Created by uniautonom on 07.08.20.
//
#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <iostream>

#include "log_analyser.h"

LogAnalyser::LogAnalyser(QWidget *parent) : QDialog(parent){
    this->setupUi(this);
    QFileDialog dialog(this);
    connect(this->load_button,SIGNAL(clicked()), this, SLOT(on_loadButton_clicked()));
    connect(this->clearLog,SIGNAL(clicked()),this,SLOT(on_clearButton_clicked()));
}

void LogAnalyser::on_loadButton_clicked(){
    QStringList fileNames;
    //    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
//    dialog.setNameFilter(tr("Images (*.png *.jpg)"));
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        fileNames = dialog.selectedFiles();
    }
//    QString file_name = QFileDialog::getOpenFileName(this, "Select log file", QDir::homePath());
    QString string = fileNames.first();
    QFile file(string);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Could not open file");
    }
    QTextStream in(&file);
    QString text = in.readAll();
    this->textBrowser->setPlainText(text);
//    QStringList history = QFileDialog::history();
//    QFileDialog::setHistory();
    QStringList history = dialog.history();
    history.append(string);
    dialog.setHistory(history);
    std::cout << history.size() << endl << std::flush;
//    this->historyBox->clear();
    this->historyBox->addItems(history);
    file.close();
}

void LogAnalyser::on_clearButton_clicked(){
    this->textBrowser->setPlainText(nullptr);
}