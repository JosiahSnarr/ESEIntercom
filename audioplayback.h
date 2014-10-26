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

    void getRecordedAudio(QBuffer&) const;

public slots:
    void onPlayerStateChanged(QAudio::State);
    void onAudioReceived(QByteArray&);

signals:
    void stoppedPlaying();

private:
    QAudioInput*  _input;
    QAudioOutput* _output;
    QBuffer       _buffer;
    QBuffer       _broadcast;

    bool _recording;
    bool _playing;
    bool _broadcastPending;

    void createAudioIO(QAudioFormat format);

};

#endif // AUDIOPLAYBACK_H
