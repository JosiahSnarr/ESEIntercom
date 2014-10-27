

#ifndef AUDIOFILTERBUFFER_H
#define AUDIOFILTERBUFFER_H

#include <QBuffer>

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
    */
    qint64 writeData(const char *data, qint64 len);

public slots:

signals:

private:
    //! The upper cut off
    int _upperThreshold;
    //! The lower cut off
    int _lowerThreshold;

};

#endif // AUDIOFILTERBUFFER_H
