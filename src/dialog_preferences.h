#ifndef DIALOG_PREFERENCES_H
#define DIALOG_PREFERENCES_H

#include <QDialog>

#include "build/UNIAutonomRemoteGUI_autogen/include/ui_dialog_preferences.h"

class PreferencesDialog : public QDialog, public Ui_dialog_preferences {
    Q_OBJECT

public:
    PreferencesDialog(QWidget *parent = nullptr);
public slots:
    void savePreferences();
};

#endif // DIALOG_PREFERENCES_H
