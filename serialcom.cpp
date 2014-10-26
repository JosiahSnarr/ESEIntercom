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
    resetBuffer(_receiveBuffer);
    _isProcessingPacket = false;
}

void SerialCom::onDataReceived()
{
    // store in the cumulative buffer
    _receiveBuffer.write(_serial->readAll());

    // check if there is either data for a packet header
    if(_isProcessingPacket == false && _receiveBuffer.size() >= sizeof(FrameHeader)){
        qDebug() << "Getting packet header";

        // go to the start of the buffer
        _receiveBuffer.reset();

        // read the frame header
        _receiveBuffer.read((char*)&_inHeader, sizeof(FrameHeader));

        // verify the packet is valid
        if(_inHeader.lSignature == FRAME_SIGNATURE){
            qDebug() << "Valid header received\n";
            qDebug() << "Will wait for " << _inHeader.lDataLength << " bytes";
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

        qDebug() << _receiveBuffer.size();

        // check for the number of bytes specified by the header
        if(_receiveBuffer.size() >= _inHeader.lDataLength){
            qDebug() << "Enough data received";

            // move to the front of the buffer for reading
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
                    emit onQueueUpdate(_queue.size);

                    qDebug() << message->msg << "\n";
                }

            }else{
                qDebug() << "No compression";

                // Uncompressed Text Message
                if(isBitSet(_inHeader.bDecodeOpts, MSG_TYPE_TEXT)){

                    Message* message = (Message*) malloc(sizeof(Message));
                    _receiveBuffer.read((char*)&message, sizeof(Message));

                    enQueue(&_queue, message);
                    emit onQueueUpdate(_queue.size);

                    qDebug() << message->msg << "\n";

                }
                // Uncompressed Audio Message
                else if(isBitSet(_inHeader.bDecodeOpts, MSG_TYPE_AUDIO)){
                    qDebug() << "Receive Uncompressed Audio Message";

                    // send the audio buffer to the broadcast player
                    QByteArray audioBuffer = _receiveBuffer.read(_inHeader.lDataLength);
                    emit onAudioReceived(audioBuffer);

                }
            }

            // finished packet processing
            _isProcessingPacket = false;
            removeProcessedData(_receiveBuffer, _inHeader.lDataLength);
        }

    }

}

void SerialCom::write(QByteArray buffer, uint8_t receiverId, bool useHeader, uint8_t decodeOptions)
{
    qDebug() << "Serial Write";
    QBuffer outData;
    outData.open(QIODevice::WriteOnly);

    FrameHeader outHeader;
    outHeader.lSignature = FRAME_SIGNATURE;
    outHeader.bVersion = 1;

    if(useHeader){
        qDebug() << "Using Framed Data";
        // fill initial header data
        outHeader.bReceiverId = receiverId;

        outHeader.bDecodeOpts = 0;
        set(outHeader.bDecodeOpts, decodeOptions);

        // handle text message
        if(isBitSet(decodeOptions, MSG_TYPE_TEXT)){
            qDebug() << "Sending Text";

            // copy buffer into the message structure
            Message message;
            message.receiverID = receiverId;
            message.priority = 1;
            message.senderID = 42;

            memcpy(message.msg, buffer.data(), BUFFER_MAX);

            // check if doing compression
            if(isBitSet(decodeOptions, COMPRESS_TYPE_HUFF) || isBitSet(decodeOptions, COMPRESS_TYPE_RLE)){

                // Run length encoding
                if(isBitSet(decodeOptions, COMPRESS_TYPE_RLE)){
                    qDebug() << "RL Encoding";

                    uint8_t* encodedBuffer = (uint8_t*) malloc(sizeof(Message));
                    int iEncodeLen = rlencode((uint8_t*)&message, sizeof(Message), encodedBuffer, sizeof(Message), 27);

                    outHeader.lUncompressedLength = sizeof(Message);
                    outHeader.lDataLength = iEncodeLen;

                    outData.write((char*)&outHeader, sizeof(FrameHeader));
                    outData.write((char*)encodedBuffer, iEncodeLen);

                }
                if(isBitSet(decodeOptions, COMPRESS_TYPE_HUFF)){

                }
            }
            // no compression of a message
            else{
                qDebug() << "No Compression";
                outHeader.lUncompressedLength = sizeof(Message);
                outHeader.lDataLength = sizeof(Message);
                outData.write((char*)&outHeader, sizeof(FrameHeader));
                outData.write((char*)&message, sizeof(Message));
            }

        }
        // handle audio message
        else if(isBitSet(decodeOptions, MSG_TYPE_AUDIO)){

            if(isBitSet(decodeOptions, COMPRESS_TYPE_HUFF) || isBitSet(decodeOptions, COMPRESS_TYPE_RLE)){

            }
            // uncompressed audio
            else{
                outHeader.lDataLength = buffer.length();
                outHeader.lUncompressedLength = buffer.length();
                outData.write((char*)&outHeader, sizeof(FrameHeader));
                outData.write(buffer);
            }

        }
    }
    else{
        outData.write(buffer);
    }

    // write out to serial
    qDebug() << "written bytes: " << _serial->write(outData.buffer()) << "\n";
    outData.close();
    //_serial->flush();
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
        // move to the end of the data
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
