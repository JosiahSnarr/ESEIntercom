#include "serialcom.h"
#include <QByteArray>
#include <QDebug>

#include <string>

SerialCom::SerialCom(QObject *parent) :
    QObject(parent)
{
    _serial = new QSerialPort(this);
    connect(_serial, SIGNAL(readyRead()), this, SLOT(onDataReceived()));

    _header.lSignature = FRAME_SIGNATURE;
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

        FrameHeader header;
        _serial->read((char*)&header, sizeof(FrameHeader));

        if(header.lSignature == FRAME_SIGNATURE){
            qDebug() << "Valid Signature\n";
            qDebug() << "Receiver: " << header.bReceiverId << "\n";
            qDebug() << "Data Length: " << header.lDataLength << "\n";

            qDebug() << "available: " << _serial->bytesAvailable() << "\n";

            if(header.lDataLength <= _serial->bytesAvailable()){

                if(header.lDataLength == sizeof(Message)){

                    Message* message = (Message*) malloc(sizeof(Message));
                    _serial->read((char*)&message, sizeof(Message));

                    enQueue(&_queue, message);

                    qDebug() << message->msg << "\n";

                }

            }
            else{
                qDebug() << "not enough sent! problem!\n";
            }

        }
        else{
            qDebug() << "Corrupt data!\n";
            qDebug() << "available: " << _serial->bytesAvailable() << "\n";
        }

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

void SerialCom::write(QByteArray stringBuffer, uint8_t receiverId)
{
    QBuffer outData;
    outData.open(QIODevice::WriteOnly);

    // fill some header data
    _header.bReceiverId = receiverId;
    _header.lDataLength = sizeof(Message);

    // frame the data
    outData.write((char*)&_header, sizeof(FrameHeader));

    qDebug() << "stringbuffer len: " << stringBuffer.length() << "\n";

    Message message;
    message.priority = 1;
    message.msgSeq = 7;
    message.receiverID = receiverId;
    message.senderID = 99;

    memcpy(message.msg, stringBuffer.data(), BUFFER_MAX);

    qDebug() << "message: " << message.msg << "\n" << "msg len: " << strlen(message.msg) << "\n";

    outData.write((char*)&message, sizeof(Message));

    // write out to serial
    _serial->write(outData.buffer());
}

void SerialCom::frameBuffer(QByteArray& buffer)
{
    // add the frame header to the serial output buffer

}

SerialCom::~SerialCom()
{
    delete _serial;
    deleteQueue(&_queue);
}
