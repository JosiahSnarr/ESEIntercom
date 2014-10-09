#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>
#include <QJsonObject>
#include <QString>
#include <QtSerialPort/QSerialPort>

#define FILE_SERIAL_CONFIG "serial.json"
#define PORTNAME    "PortName"
#define BAUD        "BaudRate"
#define PARITY      "Parity"
#define STOPBITS    "StopBits"
#define DATABITS    "DataBits"
#define FLOWCONTROL "FlowControl"

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
        QString portName;
        QSerialPort::BaudRate baudrate;
        QSerialPort::Parity parity;
        QSerialPort::DataBits databits;
        QSerialPort::StopBits stopbits;
        QSerialPort::FlowControl flowcontrol;
    };

public:
    Settings getSettings() const;

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
