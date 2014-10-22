
/**
    Main communication interface to RS232

    @author Natesh Narain
*/

#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <cstdint>

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QBuffer>

#include "serialsettings.h"
#include "messagequeue.h"

#define FRAME_SIGNATURE 0xDEADBEEF
#define DEBUG_SERIAL_OUT QString("DEADBEEF")

#define READY_READ_SIZE sizeof(FrameHeader) + sizeof(Message)

typedef struct header{
    uint32_t lSignature;
    uint32_t lDataLength;
    uint8_t  bReceiverId;
    uint8_t  bVersion;
    uint8_t  bPattern;
    uint8_t  bTBD[21];
}FrameHeader;

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
    void write(QByteArray data, uint8_t receiverId);

    /**
    */
    void frameBuffer(QByteArray& buffer);

private:
    //! serial port access
    QSerialPort* _serial;
    QBuffer _receiveBuffer;

    FrameHeader _header;

    MessageQueue _queue;

};

#endif // SERIALCOM_H
