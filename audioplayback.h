#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include <QObject>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioEncoderSettings>

class AudioPlayback : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayback(QAudioEncoderSettings format, QObject *parent = 0);
    ~AudioPlayback(void);

    void record();
    void stopRecording();
    void play();
    void stopPlayback();

    void setAudioFormat(QAudioFormat format);
    void setAudioFormat(QAudioEncoderSettings settings);

    bool isRecording();
    bool isPlaying();

private:
    QAudioInput*  _input;
    QAudioOutput* _output;
    QBuffer       _buffer;

    bool _recording;
    bool _playing;

    void createAudioIO(QAudioFormat format);

signals:
    void stoppedPlaying();

public slots:
    void onPlayerStateChanged(QAudio::State);

};

#endif // AUDIOPLAYBACK_H
