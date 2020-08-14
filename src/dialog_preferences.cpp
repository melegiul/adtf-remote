#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include "dialog_preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
    this->setupUi(this);

    this->tabWidget->setCurrentIndex(0);
    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::savePreferences);
    connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
    connect(this->pathApplyButton, SIGNAL(clicked()), this, SLOT(handleApplyPathButton()));
    connect(this->choosePathButton, SIGNAL(clicked()), this, SLOT(handleChoosePathButton()));

    savePreferences();
    showPreferences();
}

void PreferencesDialog::loadPreferences() {
    //open dialog
    fileNameCarConfig = QFileDialog::getOpenFileName(this, "Choose car configuration ini file");
    qDebug() << "car configuration ini file" << fileNameCarConfig << "selected for opening";
    if (fileNameCarConfig.isNull()) return;
    
    QSettings settings;
    settings.setValue("car/settings", fileNameCarConfig);

    showPreferences();
}

void PreferencesDialog::showPreferences() {
    QSettings settings;
    this->le_ipaddress->setText(settings.value("network/ipaddress").toString());
    this->sb_port->setValue(settings.value("network/port", 9536).toInt());
    this->le_description->setText(settings.value("description/path").toString());
    this->car_config_edit_label->setText(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString());
    this->sb_background->setValue(settings.value("ui/background", 180).toInt());
    QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);
    this->sb_car_init_pos_x->setValue(carSettings.value("odoinit/posx", 200.0f).toFloat());
    this->sb_car_init_pos_y->setValue(carSettings.value("odoinit/posy", 200.0f).toFloat());
    this->sb_car_init_orientation->setValue(carSettings.value("odoinit/orientation", 0.1f).toFloat());
    this->sb_car_length->setValue(carSettings.value("car/length", 400).toInt());
    this->sb_car_width->setValue(carSettings.value("car/width", 240).toInt());
    this->sb_left_near_x->setValue(carSettings.value("carview/leftnearx", -300.0f).toFloat());
    this->sb_left_near_y->setValue(carSettings.value("carview/leftneary", 300.0f).toFloat());
    this->sb_right_near_x->setValue(carSettings.value("carview/rightnearx", 300.0f).toFloat());
    this->sb_right_near_y->setValue(carSettings.value("carview/rightneary", 300.0f).toFloat());
    this->sb_left_far_x->setValue(carSettings.value("carview/leftfarx", -300.0f).toFloat());
    this->sb_left_far_y->setValue(carSettings.value("carview/leftfary", 600.0f).toFloat());
    this->sb_right_far_x->setValue(carSettings.value("carview/rightfarx", 300.0f).toFloat());
    this->sb_right_far_y->setValue(carSettings.value("carview/rightfary", 600.0f).toFloat());
}

void PreferencesDialog::savePreferences() {
    QSettings settings;
    settings.setValue("network/ipaddress", this->le_ipaddress->text());
    settings.setValue("network/port", this->sb_port->value());
    settings.setValue("description/path", this->le_description->text());
    settings.setValue("ui/background", this->sb_background->value());
    settings.setValue("car/settings", this->car_config_edit_label->text() == "" ? "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini" : this->car_config_edit_label->text());

    settings.setValue("logview/logPath", this->logPathLineEdit->text());
    settings.setValue("logview/automaticSave", this->stopButton->isChecked() ? \
    QString("stop"): QString("abort"));

    QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);
    carSettings.setValue("odoinit/posx", this->sb_car_init_pos_x->value());
    carSettings.setValue("odoinit/posy", this->sb_car_init_pos_y->value());
    carSettings.setValue("odoinit/orientation", this->sb_car_init_orientation->value());
    carSettings.setValue("car/length", this->sb_car_length->value());
    carSettings.setValue("car/width", this->sb_car_width->value());
    carSettings.setValue("carview/leftnearx", this->sb_left_near_x->value());
    carSettings.setValue("carview/leftneary", this->sb_left_near_y->value());
    carSettings.setValue("carview/rightnearx", this->sb_right_near_x->value());
    carSettings.setValue("carview/rightneary", this->sb_right_near_y->value());
    carSettings.setValue("carview/leftfarx", this->sb_left_far_x->value());
    carSettings.setValue("carview/leftfary", this->sb_left_far_y->value());
    carSettings.setValue("carview/rightfarx", this->sb_right_far_x->value());
    carSettings.setValue("carview/rightfary", this->sb_right_far_y->value());
}

void PreferencesDialog::handleApplyPathButton() {
    QString logPath = this->logPathLineEdit->text();
    QDir directory(logPath);
    if (!directory.exists() || logPath.isEmpty()){
        QMessageBox::warning(this, "Warning", "Entered text is not a valid directory!");
    } else {
        QMessageBox::information(this, "Confirmed", "Directory confirmed");
    }
}

void PreferencesDialog::handleChoosePathButton(){
    QStringList dirNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        dirNames = dialog.selectedFiles();
    }
    QString directory = dirNames.first();
    this->logPathLineEdit->setText(directory);
}
