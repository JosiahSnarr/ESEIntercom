#include "audiorecorder.h"

AudioRecorder::AudioRecorder(QObject *parent) :
    QObject(parent)
{
    playerBuffer = new QBuffer(this);
//    playerBuffer->open(QIODevice::ReadWrite);

    recorder = new QAudioRecorder(this);
    recorder->setAudioInput("Default");
    probe = new QAudioProbe(this);

    probe->setSource(recorder);

    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));

    player = NULL;

    recording = false;
    listening = false;
}

void AudioRecorder::processBuffer(QAudioBuffer buff)
{
   // qDebug() << "processBuffer(buffer)\n";
    audioBuffer = buff;
}

void AudioRecorder::record(QAudioEncoderSettings settings)
{
    // set format for recorder and player
    recorder->setEncodingSettings(settings);

    QAudioFormat format;
    format.setSampleRate(settings.sampleRate());
    format.setSampleSize(8);
    format.setChannelCount(settings.channelCount());
    format.setCodec(settings.codec());
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    // create output for current recording
    createOutput(format);

    // record
    recorder->record();
    recording = true;
}

void AudioRecorder::stopRecording()
{
    recorder->stop();
    recording = false;
}

void AudioRecorder::listen()
{
    playerBuffer->setData(getBuffer());
    playerBuffer->open(QIODevice::ReadWrite);
    player->start(playerBuffer);
    listening = true;
}

void AudioRecorder::stopListening()
{
    player->stop();
    playerBuffer->seek(0);
    playerBuffer->close();
    listening = false;
}

void AudioRecorder::playbackStateChanged(QAudio::State state)
{
    if(state == QAudio::IdleState){
        qDebug() << "Idle\n";
    }
    else if (state == QAudio::ActiveState){
        qDebug() << "Active\n";
    }
    else if(state == QAudio::StoppedState){
        qDebug() << "Stopped\n";
    }
}

void AudioRecorder::createOutput(QAudioFormat format)
{
    if(player != NULL){
        disconnect(player, SIGNAL(stateChanged(QAudio::State)), this, SLOT(playbackStateChanged(QAudio::State)));
        delete player;
    }
    player = new QAudioOutput(format, this);
    connect(player, SIGNAL(stateChanged(QAudio::State)), this, SLOT(playbackStateChanged(QAudio::State)));
}

QByteArray AudioRecorder::getBuffer()
{
    return QByteArray((const char *)audioBuffer.data(), audioBuffer.byteCount());
}

bool AudioRecorder::isRecording()
{
    return recording;
}

bool AudioRecorder::isListening()
{
    return listening;
}

AudioRecorder::~AudioRecorder(void)
{
    delete recorder;
    delete probe;
    delete playerBuffer;

    if(player != NULL) delete player;
}
