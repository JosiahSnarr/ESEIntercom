#include "serialcom.h"
#include <QByteArray>
#include <QDebug>

SerialCom::SerialCom(QObject *parent) :
    QObject(parent)
{
    _serial = new QSerialPort(this);
    connect(_serial, SIGNAL(readyRead()), this, SLOT(onDataReceived()));

    _header.lSignature = 0xDEADBEEF;
    _header.bPattern = 0x5A;
    _header.bVersion = 1;
    _header.bTBD[0] = 0;
}

bool SerialCom::open(SerialSettings::Settings settings)
{
    _serial->setPortName(settings.portName);
    _serial->setBaudRate(settings.baudrate);
    _serial->setDataBits(settings.databits);
    _serial->setStopBits(settings.stopbits);
    _serial->setParity(settings.parity);
    _serial->setFlowControl(settings.flowcontrol);
    _serial->setReadBufferSize(sizeof(Message));

    return _serial->open(QIODevice::ReadWrite);
}

void SerialCom::close()
{
    _serial->close();
}

void SerialCom::onDataReceived()
{
    if(_serial->bytesAvailable() >= READY_READ_SIZE){
        QByteArray data = _serial->readAll();

        // allocate space for a new message and add to the message queue
        Message * message = (Message *) malloc(sizeof(Message));
        memcpy(message, data.data(), sizeof(Message));

        enQueue(&_queue, message);
    }

    // debug
    if(_serial->bytesAvailable() == DEBUG_SERIAL_OUT.length()){
        QByteArray data = _serial->readAll();
        QString msg(data);

        if(msg == DEBUG_SERIAL_OUT){
            qDebug() << "Debug message recieved" << "\n";
            qDebug() << msg << "\n";
        }
    }

}

void SerialCom::write(QByteArray data, uint8_t receiverId)
{
    QByteArray outData;

    // fill some header data
    _header.bReceiverId = receiverId;
    _header.lDataLength = (long) data.length();

    // fill the buffer
    frameBuffer(outData);
    outData.append(data);

    // write out to serial
    _serial->write(outData);
}

void SerialCom::frameBuffer(QByteArray& buffer)
{
    // add the frame header to the serial output buffer
    buffer.append((char*)&_header);
}

SerialCom::~SerialCom()
{
    delete _serial;
}
