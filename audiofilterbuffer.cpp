#include "audiofilterbuffer.h"
#include <cstdint>

#include <QDebug>

AudioFilterBuffer::AudioFilterBuffer(QObject *parent) : QBuffer(parent)
{
    _upperThreshold = Amplitude::MAX;
    _lowerThreshold = Amplitude::MIN;

    int i;
    for(i = 0; i < 256; ++i) _byteCount[i] = 0;
}

qint64 AudioFilterBuffer::writeData(const char *data, qint64 len)
{
    const uint8_t channelBytes = 1;
    const int sampleBytes = 1;
    const int numChannels = 1;

    const int numSamples = len / sampleBytes;

    uint8_t* filtered = (uint8_t*)malloc(len * sizeof(uint8_t));
    uint8_t* filterPtr = filtered;
    memcpy(filtered, data, (size_t)len);

    int i, j;
    for(i =0; i < numSamples; ++i){
        for(j = 0; j < numChannels; ++j){

            if(*filterPtr > _upperThreshold) *filterPtr = Amplitude::MAX;
            if(*filterPtr < _lowerThreshold) *filterPtr = Amplitude::MIN;

            _byteCount[*filterPtr]++;

            filterPtr += channelBytes;
        }
    }

    qint64 length = QBuffer::writeData((const char*)filtered, len);
    free(filtered);
    return length;
}

uint8_t AudioFilterBuffer::getLeastUsedByte()
{
    int smallestIdx = INT32_MAX;
    int i = 0;

    for(i = 0; i < 256; ++i){
        if(_byteCount[i] < smallestIdx){
            smallestIdx = i;
            _byteCount[i] = 0;
        }
    }

    return smallestIdx;
}

void AudioFilterBuffer::setUpperThreshold(int upper)
{
    if(upper > Amplitude::MAX) upper = Amplitude::MAX;
    _upperThreshold = upper;
}

void AudioFilterBuffer::setLowerThreshold(int lower)
{
    if(lower < Amplitude::MIN) lower = Amplitude::MIN;
    _lowerThreshold = lower;
}

uint8_t AudioFilterBuffer::getLowerThreshold() const
{
    return _lowerThreshold;
}

uint8_t AudioFilterBuffer::getUpperThreshold() const
{
    return _upperThreshold;
}

AudioFilterBuffer::~AudioFilterBuffer()
{

}
