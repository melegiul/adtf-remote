#include <QSettings>
#include <QWidget>

#include "dialog_preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
    this->setupUi(this);

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::savePreferences);

    QSettings settings;
    this->le_ipaddress->setText(settings.value("preferences/ipaddress").toString());
    this->sb_port->setValue(settings.value("preferences/port", 9546).toInt());
}

void PreferencesDialog::savePreferences() {
    QSettings settings;
    settings.setValue("preferences/ipaddress", this->le_ipaddress->text());
    settings.setValue("preferences/port", this->sb_port->value());
}
