#ifndef STREAMBUFFER_H
#define STREAMBUFFER_H

#include <QBuffer>
#include <QMutex>

class StreamBuffer : public QBuffer
{
    Q_OBJECT
public:
    explicit StreamBuffer(QObject *parent = 0);
    ~StreamBuffer(void);

    /**
        Reimplementation of QBuffer::writeData()
    */
    qint64 writeData(const char *data, qint64 len);

    /**
        Reimplementation of QBuffer::readData()
    */
    qint64 readData(char *data, qint64 maxlen);

    void removeChunk(qint64 offset);
    void resetBuffer(QByteArray&);

    qint64 readPosition() const;

signals:

public slots:

private:
    //! Position to read from
    qint64 _readPos;
    //! Thread synchronization
    QMutex* _sync;

};

#endif // STREAMBUFFER_H
