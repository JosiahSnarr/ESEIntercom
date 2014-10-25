#include "serialcom.h"
#include <QByteArray>
#include <QDebug>

#include <string>

#include "rlencoding.h"
#include "bitopts.h"

#define Q_HEXSTR(x) QString("%1").arg(x, 0, 16)

SerialCom::SerialCom(QObject *parent) :
    QObject(parent)
{
    _serial = new QSerialPort(this);
    connect(_serial, SIGNAL(readyRead()), this, SLOT(onDataReceived()));

    _outHeader.lSignature = FRAME_SIGNATURE;
    _outHeader.bVersion = 1;

    _isProcessingPacket = false;

    initQueue(&_queue);
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

    _receiveBuffer.open(QIODevice::ReadWrite);

    return _serial->open(QIODevice::ReadWrite);
}

void SerialCom::close()
{
    _serial->close();
}

void SerialCom::onDataReceived()
{
    // store in the cumulative buffer
    _receiveBuffer.write(_serial->readAll());
    qint64 size = _receiveBuffer.size();

    // check if there is either data for a packet header
    if(_isProcessingPacket == false && size >= sizeof(FrameHeader)){
        qDebug() << "Getting packet header";

        // go to the start of the buffer
        _receiveBuffer.reset();

        // read the frame header
        _receiveBuffer.read((char*)&_inHeader, sizeof(FrameHeader));

        // verify the packet is valid
        if(_inHeader.lSignature == FRAME_SIGNATURE){
            qDebug() << "Valid header received\n";

            // specify that a packet is now being processed
            _isProcessingPacket = true;
            removeProcessedData(_receiveBuffer, sizeof(FrameHeader));
        }
        else{
            resetBuffer(_receiveBuffer);
            qDebug() << "Data discarded";
        }
        qDebug() << "\n";
    }

    // check if a packet is being processed
    if(_isProcessingPacket){

        if(_receiveBuffer.size() >= _inHeader.lDataLength){
            qDebug() << "Enough data received";

            // read the amount of data specified by the header
            _receiveBuffer.reset();

            // using RLE compression
            if(isBitSet(_inHeader.bDecodeOpts, COMPRESS_TYPE_RLE)){
                qDebug() << "RL Decode";

                qDebug() << "buffer size     : " << _receiveBuffer.size();
                qDebug() << "Data Len        : " << _inHeader.lDataLength;
                qDebug() << "Uncompressed Len: " << _inHeader.lUncompressedLength;

                // create a buffer for the uncompressed data
                uint8_t* decodeBuffer = (uint8_t*) malloc(_inHeader.lUncompressedLength);

                // get the data from the buffer
                QByteArray bytes = _receiveBuffer.read(_inHeader.lDataLength);
                uint8_t* raw = (uint8_t*) bytes.data();

                // uncompress the data
                rldecode(raw, _inHeader.lDataLength, decodeBuffer, _inHeader.lUncompressedLength, 0x1B);

                //
                if(isBitSet(_inHeader.bDecodeOpts, MSG_TYPE_TEXT)){

                    Message* message = (Message*)decodeBuffer;
                    enQueue(&_queue, message);

                    qDebug() << message->msg << "\n";
                    qDebug() << message->priority;
                    qDebug() << message->receiverID;
                    qDebug() << message->senderID;

                    emit onQueueUpdate(_queue.size);
                }

            }

            // finished packet processing
            _isProcessingPacket = false;
            removeProcessedData(_receiveBuffer, _inHeader.lDataLength);
        }

    }

}

void SerialCom::write(QByteArray stringBuffer, uint8_t receiverId)
{
    QBuffer outData;
    outData.open(QIODevice::WriteOnly);

    // fill some header data
    _outHeader.bReceiverId = receiverId;
    _outHeader.bDecodeOpts = bv(MSG_TYPE_TEXT) | bv(COMPRESS_TYPE_RLE) | bv(ENCRYPT_TYPE_NONE);

    // write message data
    Message message;
    message.priority = 1;
    message.msgSeq = 7;
    message.receiverID = receiverId;
    message.senderID = 42;

    memcpy(message.msg, stringBuffer.data(), BUFFER_MAX);

    qDebug() << message.msg << "\n";

    // RL Encode the data to be sent
    uint8_t* encodedBuffer = (uint8_t*) malloc(sizeof(Message));
    int iEncodeLen = rlencode((uint8_t*)&message, sizeof(Message), encodedBuffer, sizeof(Message), 0x1B);

    qDebug() << "I/P Data Size: " << sizeof(Message);
    qDebug() << "Encoded Buffer Size: " << iEncodeLen;
    qDebug() << "Compression Ratio: " << ((float)sizeof(Message)/(float)iEncodeLen) << "\n";

    _outHeader.lDataLength = iEncodeLen;
    _outHeader.lUncompressedLength = sizeof(Message);

    // write framed data to the out Buffer
    outData.write((char*)&_outHeader, sizeof(FrameHeader));
    outData.write((char*)encodedBuffer, iEncodeLen);

    // write out to serial
    outData.close();
    qDebug() << "written bytes: " << _serial->write(outData.buffer()) << "\n";
    _serial->flush();

    free(encodedBuffer);
}

Message* SerialCom::getNextMessageFromQueue()
{
    Message* message = deQueue(&_queue);
    emit onQueueUpdate(_queue.size);
    return message;
}

void SerialCom::removeProcessedData(QBuffer& buffer, qint64 offset)
{
    // find the number of bytes after the offset
    qint64 remainingBytes = buffer.size() - offset;

    if(remainingBytes > 0){

        buffer.reset();

        // seek to the start of the chunk that is going to be moved
        buffer.seek(offset);
        // put that chunk in a byte array
        QByteArray array = buffer.read(remainingBytes);

        // clear the buffer and put the array at the start
        resetBuffer(buffer, array);
        buffer.seek(remainingBytes);
    }
    else{
        resetBuffer(buffer);
    }
}

void SerialCom::resetBuffer(QBuffer& buffer, QByteArray& array)
{
    if(buffer.isOpen()){
        // seek to start and close
        buffer.reset();
        buffer.close();
        // clear data
        buffer.setData(array);
        // reopen
        buffer.open(QIODevice::ReadWrite);
    }
    else{
        // open
        buffer.open(QIODevice::ReadWrite);
    }
}

void SerialCom::resetBuffer(QBuffer& buffer)
{
    resetBuffer(buffer, QByteArray());
}

SerialCom::~SerialCom()
{
    delete _serial;
    if(_queue.size > 0) deleteQueue(&_queue);
}
