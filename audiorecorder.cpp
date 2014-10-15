#include "audiorecorder.h"

AudioRecorder::AudioRecorder(QObject *parent) :
    QObject(parent)
{
    playerBuffer = new QBuffer(this);
//    playerBuffer->open(QIODevice::ReadWrite);

    recorder = new QAudioRecorder(this);
    probe = new QAudioProbe(this);

    probe->setSource(recorder);

    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));

    recording = false;
    listening = false;
}

void AudioRecorder::processBuffer(QAudioBuffer buff)
{
    qDebug() << "processBuffer(buffer)\n";
    buffer = buff;
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

void AudioRecorder::createOutput(QAudioFormat format)
{
    if(player == NULL){
        delete player;
    }
    player = new QAudioOutput(format, this);
}

QByteArray AudioRecorder::getBuffer()
{
    return QByteArray((const char *)buffer.data(), buffer.byteCount());
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
