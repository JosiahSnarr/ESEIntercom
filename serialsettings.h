#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>
#include <QJsonObject>

#define FILE_SERIAL_CONFIG "serial.json"
#define BAUD     "BaudRate"
#define PARITY   "Parity"
#define STOPBITS "StopBits"

namespace Ui {
class SerialSettings;
}

class SerialSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSettings(QWidget *parent = 0);
    ~SerialSettings();

    struct Settings{
        int baud;
        int parity;
        int stopbits;
    };

public slots:
    void onOKButtonClicked();
    void onSaveButtonClicked();

private:
    Ui::SerialSettings *ui;

    QJsonObject json;

    Settings settings;

    void fillParams();
    void updateSettings();
    void loadSettings();
    void saveSettings();
};

#endif // SERIALSETTINGS_H
