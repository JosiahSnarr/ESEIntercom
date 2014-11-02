#include "streambuffer.h"

StreamBuffer::StreamBuffer(QObject *parent) : QBuffer(parent)
{
    _readPos = 0;
}

qint64 StreamBuffer::writeData(const char *data, qint64 len)
{
    qint64 bytesWritten;
    _sync.lock();
    {
        // get the current position in the buffer
        qint64 currentPos = pos();
        // get the end position of the buffer
        qint64 bufferSize = size();

        // seek to the end
        seek(bufferSize - 1);

        // write data to the end of the buffer
        bytesWritten = QBuffer::writeData(data, len);

        // seek back to the current position
        seek(currentPos);
    }
    _sync.unlock();

    return bytesWritten;
}

qint64 StreamBuffer::readData(char *data, qint64 maxlen)
{
    qint64 bytesRead;
    _sync.lock();
    {
        qint64 currentPos = pos();
        seek(_readPos);
        bytesRead = QBuffer::readData(data, maxlen);
        _readPos += bytesRead;
        seek(currentPos);
    }
    _sync.unlock();

    return bytesRead;
}

StreamBuffer::~StreamBuffer()
{

}
