/**
    @file audioplayback.cpp
    @breif Record\Playback user audio. Play broadcast audio
    @author Natesh Narain
*/

#include "audioplayback.h"
#include <QAudioDeviceInfo>

#include <QDebug>

AudioPlayback::AudioPlayback(QAudioEncoderSettings format, QObject *parent) :
    QObject(parent)
{
    setAudioFormat(format);

    _recording = false;
    _playing = false;
    _broadcastPending = false;
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
    _buffer.close();
    _playing = false;
}

void AudioPlayback::onPlayerStateChanged(QAudio::State state)
{
    if(state == QAudio::StoppedState || state == QAudio::IdleState){
        stopPlayback();
        emit stoppedPlaying();
    }
}

void AudioPlayback::onAudioReceived(QByteArray& buffer)
{
    qDebug() << "Broadcast received";

    if(_playing){
        qDebug() << "Settings broadcast to pending";
        _broadcastPending = true;
        _broadcast.close();
        _broadcast.setBuffer(&buffer);
    }
    else{
        qDebug() << "Starting broadcast";

        if(_broadcast.isOpen()) _broadcast.close();
        _broadcast.setData(buffer);
        _broadcast.open(QIODevice::ReadOnly);
        _output->start(&_broadcast);
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

void AudioPlayback::getRecordedAudio(QBuffer& data) const
{
    QByteArray b = _buffer.data();
    data.setData(b);
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
