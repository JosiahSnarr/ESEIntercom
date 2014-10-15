#include "serialcom.h"
#include <QByteArray>
#include <QDebug>

SerialCom::SerialCom(QObject *parent) :
    QObject(parent)
{
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(onDataReceived()));
}

bool SerialCom::open(SerialSettings::Settings settings)
{
    serial->setPortName(settings.portName);
    serial->setBaudRate(settings.baudrate);
    serial->setDataBits(settings.databits);
    serial->setStopBits(settings.stopbits);
    serial->setParity(settings.parity);
    serial->setFlowControl(settings.flowcontrol);
    serial->setReadBufferSize(sizeof(Message));

    return serial->open(QIODevice::ReadWrite);
}

void SerialCom::close()
{
    serial->close();
}

void SerialCom::onDataReceived()
{
    if(serial->bytesAvailable() >= READY_READ_SIZE){
        QByteArray data = serial->readAll();
        QString msg(data);
        emit messageReceived(msg);
    }

    // debug
    if(serial->bytesAvailable() >= DEBUG_SERIAL_OUT.length()){
        QByteArray data = serial->readAll();
        QString msg(data);

        if(msg == DEBUG_SERIAL_OUT){
            qDebug() << "Debug message recieved" << "\n";
            qDebug() << msg << "\n";
        }
    }

}

void SerialCom::write(QByteArray data)
{
    serial->write(data);
}

SerialCom::~SerialCom()
{
    delete serial;
}
