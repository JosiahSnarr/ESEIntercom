#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include <QObject>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>

class AudioPlayback : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayback(QAudioFormat format, QObject *parent = 0);
    ~AudioPlayback(void);

    void record();
    void stopRecording();
    void play();
    void stopPlayback();

    bool isRecording();
    bool isPlaying();

private:
    QAudioInput*  _input;
    QAudioOutput* _output;
    QBuffer       _buffer;

    bool _recording;
    bool _playing;

signals:

public slots:

};

#endif // AUDIOPLAYBACK_H
