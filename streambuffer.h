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

    qint64 writeData(const char *data, qint64 len);
    qint64 readData(char *data, qint64 maxlen);

signals:

public slots:

private:
    qint64 _readPos;
    //! Thread synchronization
    QMutex _sync;

};

#endif // STREAMBUFFER_H
