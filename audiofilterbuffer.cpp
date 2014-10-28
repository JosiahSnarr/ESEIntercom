#include "audiofilterbuffer.h"
#include <cstdint>

#include <QDebug>

AudioFilterBuffer::AudioFilterBuffer(QObject *parent) : QBuffer(parent)
{
    _upperThreshold = 0xFF;
    _lowerThreshold = 0x00;

    int i;
    for(i = 0; i < 256; ++i) _byteCount[i] = 0;
}

qint64 AudioFilterBuffer::writeData(const char *data, qint64 len)
{

    const int channelBytes = 1;
    const int sampleBytes = 1;
    const int numChannels = 1;

    const int numSamples = len / sampleBytes;

    uint8_t* filtered = (uint8_t*)malloc(len * sizeof(uint8_t));
    memcpy(filtered, data, (size_t)len);

    int i, j;
    for(i =0; i < numSamples; ++i){
        for(j = 0; j < numChannels; ++j){

            if(*filtered > _upperThreshold) *filtered = 0xFF;
            if(*filtered < _lowerThreshold) *filtered = 0x00;

            _byteCount[*filtered]++;

        }
    }

    qint64 length = QBuffer::writeData((const char*)filtered, len);
    free(filtered);
    return length;

    return QBuffer::writeData(data, len);
}

uint8_t AudioFilterBuffer::getLeastUsedByte() const
{
    int smallestIdx = INT32_MAX;
    int i;

    for(i = 0; i < 256; ++i){
        if(_byteCount[i] < smallestIdx){
            smallestIdx = i;
        }
    }

    return smallestIdx;
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
