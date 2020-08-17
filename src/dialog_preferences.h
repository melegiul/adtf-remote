#ifndef DIALOG_PREFERENCES_H
#define DIALOG_PREFERENCES_H

#include <QDialog>
#include <QMessageBox>

#include "ui_dialog_preferences.h"

class PreferencesDialog : public QDialog, public Ui_dialog_preferences {
    Q_OBJECT

private:
    QString fileNameCarConfig = nullptr;
    void showPreferences();
public:
    PreferencesDialog(QWidget *parent = nullptr);
public slots:
    void loadPreferences();
    void savePreferences();
    void handleApplyPathButton();
    void handleChoosePathButton();
};

#endif // DIALOG_PREFERENCES_H
