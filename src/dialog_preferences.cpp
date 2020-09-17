#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QtDebug>

#include "dialog_preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
{
    this->setupUi(this);

    this->tabWidget->setCurrentIndex(0);
    QIntValidator *fileNumValidator = new QIntValidator(this);
    fileNumValidator->setBottom(1);
    fileNumber->setValidator(fileNumValidator);
    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::savePreferences);
    connect(this->load_car_config_button, SIGNAL(clicked()), this, SLOT(loadPreferences()));
    connect(this->pathApplyButton, SIGNAL(clicked()), this, SLOT(handleApplyPathButton()));
    connect(this->choosePathButton, SIGNAL(clicked()), this, SLOT(handleChoosePathButton()));
    connect(this->saveButton, SIGNAL(clicked()), this, SLOT(savePreferences()));
    connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(this->fileDelimiter,SIGNAL(stateChanged(int)),this,SLOT(checkDelimiter()));
    connect(this->fileNumber,SIGNAL(editingFinished()),this,SLOT(checkEditor()));

    showPreferences();
    loadLogPreferences();
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
    this->le_description->setText(settings.value("description/path", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/description/uni_autonom.description").toString());
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

/**
 * loads settings into gui
 */
void PreferencesDialog::loadLogPreferences() {
    QString stdPath = QDir::currentPath();
    // retrieve cmake-build-debug directory path
    QDir appDir = QDir::current();
    // modify path to default location for saving logs
    appDir.cdUp();
    appDir.cdUp();
    QString appDirPath = appDir.path() + "/json";
    this->logPathLineEdit->setText(settings.value("logPreferences/logPath", appDirPath).toString());
    settings.value("logPreferences/automaticSave") == QString("stop") ? \
                    this->stopButton->setChecked(true): this->abortButton->setChecked(true);
    this->fileDelimiter->setChecked(settings.value("logPreferences/fileDelimiter", false).toBool() ? true: false);
    this->fileNumber->setText(fileDelimiter->isChecked() ? \
                    settings.value("logPreferences/fileNumber").toString(): "");
}

void PreferencesDialog::savePreferences() {
    QSettings settings;
    settings.setValue("network/ipaddress", this->le_ipaddress->text());
    settings.setValue("network/port", this->sb_port->value());
    settings.setValue("description/path", this->le_description->text());
    settings.setValue("ui/background", this->sb_background->value());
    settings.setValue("car/settings", this->car_config_edit_label->text() == "" ? "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini" : this->car_config_edit_label->text());

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
    saveLogPreferences();
    reject();
}

/**
 * checks validity of user input in line edit and reports to user
 * does not save the setting
 */
void PreferencesDialog::handleApplyPathButton() {
    QString logPath = this->logPathLineEdit->text();
    QDir directory(logPath);
    if (!directory.exists() || logPath.isEmpty()){
        QMessageBox::warning(this, "Warning", "Entered text is not a valid directory!");
    } else {
        QMessageBox::information(this, "Confirmed", "Directory confirmed");
    }
}

/**
 * opens file dialog to guide the user through memory location selection
 * for log serialization
 */
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

/**
 * enabling/disabling line editor for max file number
 */
void PreferencesDialog::checkDelimiter(){
    if (fileDelimiter->isChecked()) {
        fileNumber->setEnabled(true);
        fileNumber->setText(settings.value("logPreferences/fileNumber", INT_MAX).toString());
    }else {
        fileNumber->setEnabled(false);
        fileNumber->clear();
    }
}

/**
 * prints warning and sets default value for file number delimiter,
 * if file delimiter is enabled but no value specified
 */
void PreferencesDialog::checkEditor() {
    if (fileNumber->text().isEmpty()){
        QMessageBox::warning(this,"Line edit empty", "Reset max file number to default");
        fileNumber->setText(std::to_string(INT_MAX).data());
    }
}

/**
 * saves settings and handle invalid user input
 */
void PreferencesDialog::saveLogPreferences() {
    QString logPath = this->logPathLineEdit->text();
    QDir directory(logPath);
    if (!directory.exists() || logPath.isEmpty()) {
        // enter if folder is not valid, does not apply changes
        QMessageBox::warning(this, "Warning", "Entered log path is not a valid directory!\nChanges were not applied");
    }else {
        // enters if folder is valid and saves setting
        settings.setValue("logPreferences/logPath", this->logPathLineEdit->text());
    }
    settings.setValue("logPreferences/automaticSave", this->stopButton->isChecked() ? \
                    QString("stop"): QString("abort"));
    settings.setValue("logPreferences/fileDelimiter", this->fileDelimiter->isChecked());
    if (fileDelimiter->isChecked())
        checkEditor();
    settings.setValue("logPreferences/fileNumber", this->fileDelimiter->isChecked() ? \
                    this->fileNumber->text(): QString(std::to_string(INT_MAX).data()));
}