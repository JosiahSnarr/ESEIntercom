#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QByteArray>
#include <QAudioOutput>
#include <QBuffer>

class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorder(QObject *parent = 0);
    ~AudioRecorder(void);

signals:

public slots:
    void processBuffer(QAudioBuffer);
    void playbackStateChanged(QAudio::State);

public:
    void record(QAudioEncoderSettings settings);
    void listen();

    QByteArray getBuffer();

    void stopRecording();
    void stopListening();

    bool isRecording();
    bool isListening();

private:
    QAudioRecorder* recorder;
    QAudioProbe* probe;

    QAudioBuffer audioBuffer;

    QAudioOutput* player;
    QBuffer* playerBuffer;

    bool recording;
    bool listening;

    void createOutput(QAudioFormat format);

};

#endif // AUDIORECORDER_H
