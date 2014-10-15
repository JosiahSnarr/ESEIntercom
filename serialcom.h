
/**
    Main communication interface to RS232

    @author Natesh Narain
*/

#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

#include "serialsettings.h"
#include "messagequeue.h"

#define DEBUG_SERIAL_OUT QString("DEADBEEF")

#define READY_READ_SIZE sizeof(Message)

/**
    Serial communication interface.

    Handle read, write, queuing, framing, etc
*/
class SerialCom : public QObject
{
    Q_OBJECT
public:
    explicit SerialCom(QObject *parent = 0);
    ~SerialCom();

signals:
    void messageReceived(QString);

public slots:
    /**
        Called when there is data to be read from the serial buffer
    */
    void onDataReceived();

public:
    /**
        Opens the serial port

        @param settings
            The Serial settings to configure the serial port with

        @return true for success, false otherwise
    */
    bool open(SerialSettings::Settings settings);

    /**
        Close the serial port
    */
    void close();

    /**
        Write data to the serial port

        @param data
            data to be written to the serial port
    */
    void write(QByteArray data);

private:
    //! serial port access
    QSerialPort* serial;
    MessageQueue queue;

};

#endif // SERIALCOM_H
