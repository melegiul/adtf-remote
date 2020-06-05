#include <QSettings>
#include <QWidget>

#include "dialog_preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
    this->setupUi(this);

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::savePreferences);

    QSettings settings;
    this->le_ipaddress->setText(settings.value("network/ipaddress").toString());
    this->sb_port->setValue(settings.value("network/port", 9536).toInt());
    this->sb_car_length->setValue(settings.value("car/length", 400).toInt());
    this->sb_car_width->setValue(settings.value("car/width", 240).toInt());
    this->sb_background->setValue(settings.value("ui/background", 180).toInt());
}

void PreferencesDialog::savePreferences() {
    QSettings settings;
    settings.setValue("network/ipaddress", this->le_ipaddress->text());
    settings.setValue("network/port", this->sb_port->value());
    settings.setValue("car/length", this->sb_car_length->value());
    settings.setValue("car/width", this->sb_car_width->value());
    settings.setValue("ui/background", this->sb_background->value());
}
