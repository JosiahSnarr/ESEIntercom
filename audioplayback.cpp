#include "audioplayback.h"
#include <QAudioDeviceInfo>

#include <QDebug>

AudioPlayback::AudioPlayback(QAudioEncoderSettings format, QObject *parent) :
    QObject(parent)
{
    setAudioFormat(format);

    _recording = false;
    _playing = false;
}

void AudioPlayback::record()
{
    qDebug() << "Recording\n";
    _buffer.open(QIODevice::WriteOnly);
    _input->start(&_buffer);
    _recording = true;
}

void AudioPlayback::stopRecording()
{
    qDebug() << "Stop Recording\n";
    _input->stop();
    _buffer.close();
    _recording = false;
}

void AudioPlayback::play()
{
    qDebug() << "Play\n";
    _buffer.open(QIODevice::ReadOnly);
    _buffer.seek(0);
    _output->start(&_buffer);
    _playing = true;
}

void AudioPlayback::stopPlayback()
{
    qDebug() << "Stop Play\n";
    _output->stop();
    _buffer.close();
    _playing = false;
}

void AudioPlayback::onPlayerStateChanged(QAudio::State state)
{
    if(state == QAudio::StoppedState || state == QAudio::IdleState){
        emit stoppedPlaying();
    }
}

void AudioPlayback::createAudioIO(QAudioFormat format)
{
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    _input = new QAudioInput(info, format, this);

    _output = new QAudioOutput(format, this);
    connect(_output, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onPlayerStateChanged(QAudio::State)));
}

void AudioPlayback::setAudioFormat(QAudioFormat format)
{
    if(_input == NULL) delete _input;
    if(_output == NULL) delete _output;

    createAudioIO(format);
}

void AudioPlayback::setAudioFormat(QAudioEncoderSettings settings)
{
    QAudioFormat format;
    format.setSampleRate(settings.sampleRate());
    format.setSampleSize(8);
    format.setChannelCount(settings.channelCount());
    format.setCodec(settings.codec());
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    setAudioFormat(format);
}

bool AudioPlayback::isRecording()
{
    return _recording;
}

bool AudioPlayback::isPlaying()
{
    return _recording;
}

AudioPlayback::~AudioPlayback()
{
    delete _input;
    delete _output;
}
