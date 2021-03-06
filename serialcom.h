
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
#include <QDateTime>

#include "serialsettings.h"
#include "messagequeue.h"
#include "phonebook.h"

#define FRAME_SIGNATURE 0xDEADBEEF
#define DEBUG_SERIAL_OUT QString("DEADBEEF")

#define READY_READ_SIZE sizeof(FrameHeader)

#define MSG_TYPE_TEXT         0x00 ///< Message is a text message
#define MSG_TYPE_AUDIO        0x01 ///< Message is audio
#define MSG_TYPE_AUDIO_STREAM 0x02 ///< Message is streaming audio

#define ENCRYPT_TYPE_NONE     0x03 ///< No encryption
#define ENCRYPT_TYPE_XOR      0x04 ///< XOR encryption

#define COMPRESS_TYPE_NONE    0x05 ///< No compression
#define COMPRESS_TYPE_RLE     0x06 ///< Run Length Encoding Compression
#define COMPRESS_TYPE_HUFF    0x07 ///< Huffman Encoding Compression

//! Packet header for the outgoing data
typedef struct frameHeader{
    uint32_t lSignature;          ///< Signature to verify the packet
    uint32_t lSignature2;         ///< Signature to verify the packet
    uint32_t lDataLength;         ///< length of data after the header
    uint32_t lUncompressedLength; ///< Length of the uncompressed data
    uint8_t  bReceiverId;         ///< the id of the receiver
    uint8_t  bVersion;            ///< the header version
    uint8_t  bEncryptionKey;      ///< the xor encryption key
    uint8_t  bDecodeOpts;         ///< Flags to specify how to decode message
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
    /**
        Emmitted when the data in the queue changes

        @param numQueued
            The number of messages in the queue
    */
    void onQueueUpdate(int numQueued);

    /**
        Emmitted when an audio broadcast is received
    */
    void onAudioReceived(QByteArray&);

    /**
        Emmitted when part of an audio stream is received
    */
    void onAudioStreamReceived(QByteArray&);

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
    void write(QByteArray data, uint8_t receiverId, bool useHeader, uint8_t decodeOptions);

    /**
        @return the next message in the queue
    */
    Message* getNextMessageFromQueue();

    /**
        Calculate a checksum of the data

        @param bytes
            pointer to data

        @param len
            length of data

        @param divisor
            the checksum divisor
    */
    uint8_t checksum(uint8_t *bytes, int len, uint8_t divisor);

    /**
        Set to frame the data
    */
    void setUseHeader(bool use);

    /**
        @return true if using a header
    */
    bool isUsingHeader() const;

    /**
        Get a pointer to the phone log
    */
    PhoneLog* getPhoneLog();

    /**
        Set the id of this station

        @param id
            id to change to
    */
    void setStationId(int id);

private:
    //! serial port access
    QSerialPort* _serial;
    //! serial data buffer
    QBuffer _receiveBuffer;

    //! Header of the frame currently in process
    FrameHeader _inHeader;
    //! flag indicating whether a packet is currently being processed
    bool _isProcessingPacket;

    //! FInd the header before processing data
    bool _useHeader;

    //! queue for the incoming messages
    MessageQueue _queue;
    //! added to log time and number of messages per sender
    PhoneLog _log;

    //! ID of this station
    int _stationId;

    //! Divisor used for checksum
    uint8_t _checksumDivisor;

    /**
        XOR encrypt

        @param outBuffer
            buffer to store encrypted data

        @param buffer
            buffer of data to encrypt

        @param key
            XOR key
    */
    void encryptXOR(QBuffer& outBuffer, QBuffer& buffer, uint8_t key);

    /**
        XOR encrypt

        @param outBuffer
            buffer to store encrypted data

        @param data
            pointer to data

        @param len
            length of data

        @param key
            XOR key
    */
    void encryptXOR(QBuffer& outBuffer, uint8_t* data, int len, uint8_t key);

    /**
        Remove bytes [0, offset] from the buffer and move [offset, remaining] to the start.

        @param offset
            offset specifing the amout to remove
    */
    void removeProcessedData(QBuffer& buffer, qint64 offset);

    /**
        Clear the serial buffer
    */
    void resetBuffer(QBuffer& buffer);

    /**
        Reset the buffer with the QByteArray
    */
    void resetBuffer(QBuffer& buffer, QByteArray& array);

};

#endif // SERIALCOM_H
