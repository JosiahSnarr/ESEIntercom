#include "audiofilterbuffer.h"

AudioFilterBuffer::AudioFilterBuffer(QObject *parent) :
    QBuffer(parent)
{
}

qint64 AudioFilterBuffer::writeData(const char *data, qint64 len)
{
    return QBuffer::writeData(data, len);
}

void AudioFilterBuffer::setUpperThreshold(int upper)
{
    _upperThreshold = upper;
}

void AudioFilterBuffer::setLowerThreshold(int lower)
{
    _lowerThreshold = lower;
}

AudioFilterBuffer::~AudioFilterBuffer()
{

}
