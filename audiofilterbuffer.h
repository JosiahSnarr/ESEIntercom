

#ifndef AUDIOFILTERBUFFER_H
#define AUDIOFILTERBUFFER_H

#include <QBuffer>
#include <cstdint>

/**
    Filter incoming audio stream with upper and lower cut offs to make compression easier
*/
class AudioFilterBuffer : public QBuffer
{
    Q_OBJECT
public:
    explicit AudioFilterBuffer(QObject *parent = 0);
    ~AudioFilterBuffer(void);

    enum Amplitude{
        MAX = 0xFE, MIN = 0x00
    };

    /**
        Threshold to where any greater value will be set equal to the max value

        @param upper
            The upper limit
    */
    void setUpperThreshold(int upper);

    /**
        The value to where any lower value will be set equal to the min value

        @param lower
            The lower limit
    */
    void setLowerThreshold(int lower);

    /**
        Get the least used byte
    */
    uint8_t getLeastUsedByte();

    /**
        Reimplemented QBuffer::writeData(const char *data, qint64 len)

        Used to filter the audio stream as it's being recorded
    */
    qint64 writeData(const char *data, qint64 len);

    /**
        @return the upper filter threshold
    */
    uint8_t getUpperThreshold() const;

    /**
        @return the lower filter threshold
    */
    uint8_t getLowerThreshold() const;

public slots:

signals:

private:
    //! The upper cut off
    uint8_t _upperThreshold;
    //! The lower cut off
    uint8_t _lowerThreshold;

    //! holds the count of each byte
    int _byteCount[256];

};

#endif // AUDIOFILTERBUFFER_H
