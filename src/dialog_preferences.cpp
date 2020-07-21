#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include "dialog_preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
    this->setupUi(this);

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::savePreferences);
    connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));

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
    this->car_config_edit_label->setText(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString());
    this->sb_background->setValue(settings.value("ui/background", 180).toInt());
    QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);
    this->sb_car_init_pos_x->setValue(carSettings.value("odoinit/posx", 200).toInt());
    this->sb_car_init_pos_y->setValue(carSettings.value("odoinit/posy", 200).toInt());
    this->sb_car_init_orientation->setValue(carSettings.value("odoinit/orientation", 1).toInt());
    this->sb_car_length->setValue(carSettings.value("car/length", 400).toInt());
    this->sb_car_width->setValue(carSettings.value("car/width", 240).toInt());
}

void PreferencesDialog::savePreferences() {
    QSettings settings;
    settings.setValue("network/ipaddress", this->le_ipaddress->text());
    settings.setValue("network/port", this->sb_port->value());
    settings.setValue("ui/background", this->sb_background->value());
    settings.setValue("car/settings", this->car_config_edit_label->text());

    QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);
    carSettings.setValue("odoinit/posx", this->sb_car_init_pos_x->value());
    carSettings.setValue("odoinit/posy", this->sb_car_init_pos_y->value());
    carSettings.setValue("odoinit/orientation", this->sb_car_init_orientation->value());
    carSettings.setValue("car/length", this->sb_car_length->value());
    carSettings.setValue("car/width", this->sb_car_width->value());
}
