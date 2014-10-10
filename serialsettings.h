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

/**
    Dialog to configure serial communication settings
*/
class SerialSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSettings(QWidget *parent = 0);
    ~SerialSettings();

    /**
        Contains all the serial com options
    */
    struct Settings{
        QString portName;                     ///< Name of the serial port
        QSerialPort::BaudRate baudrate;       ///< Baud Rate
        QSerialPort::Parity parity;           ///< Parity bits
        QSerialPort::DataBits databits;       ///< Data bits
        QSerialPort::StopBits stopbits;       ///< Number of stop bits
        QSerialPort::FlowControl flowcontrol; ///< Flow Control
    };

public:
    /**
        Get the serial settings
    */
    Settings getSettings() const;

public slots:
    /**
        Slot called when OK button is pressed
    */
    void onOKButtonClicked();
    /**
        Slot called when Save button is pressed
    */
    void onSaveButtonClicked();

private:
    Ui::SerialSettings *ui;

    //! Used to save configuration to a file
    QJsonObject json;

    Settings settings;

    /**
        Fill the serial configuration parameters
    */
    void fillParams();
    /**
        Update the settings
    */
    void updateSettings();
    /**
        Load settings from the file
    */
    void loadSettings();
    /**
        Save config to file
    */
    void saveSettings();
};

#endif // SERIALSETTINGS_H
