#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QDialog>

namespace Ui {
class AdvancedSettings;
}

class AdvancedSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = 0);
    ~AdvancedSettings();

public slots:
    void onOkButtonClicked();
    void onSaveButtonClicked();

    void onCompressNoneCheckboxChecked();
    void onEncryptNoneCheckboxChecked();

    void onCompressCheckboxChecked();
    void onEncryptCheckboxChecked();

signals:

private:
    Ui::AdvancedSettings *ui;
};

#endif // ADVANCEDSETTINGS_H
