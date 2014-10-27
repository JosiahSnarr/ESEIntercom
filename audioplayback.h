#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include <QObject>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioEncoderSettings>

#include "audiofilterbuffer.h"

/**
    Audio Recording, Playback and Broadcasts
*/
class AudioPlayback : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayback(QAudioEncoderSettings format, QObject *parent = 0);
    ~AudioPlayback(void);

    /**
        Start recording audio
    */
    void record();

    /**
        Stop recording audio
    */
    void stopRecording();

    /**
        Playback recorded audio
    */
    void play();

    /**
        Stop playing audio
    */
    void stopPlayback();

    /**
        Set the audio format

        @param format
            The format to use
    */
    void setAudioFormat(QAudioFormat format);

    /**
        Set the audio format

        @param setting
            The format to use
    */
    void setAudioFormat(QAudioEncoderSettings format);

    /**
        @return whether currently recording or not
    */
    bool isRecording();

    /**
        @return whether currently recording or not
    */
    bool isPlaying();

    /**
        Get the recorded audio

        @param buffer
            buffer to copy the audio into
    */
    void getRecordedAudio(QBuffer& buffer) const;

public slots:
    void onPlayerStateChanged(QAudio::State);
    void onAudioReceived(QByteArray&);

signals:
    void stoppedPlaying();

private:
    //! recording
    QAudioInput*  _input;
    //! playing
    QAudioOutput* _output;
    //! buffer to hold recorded data
    AudioFilterBuffer _buffer;
    //! buffer used to hold broadcasted audio
    QBuffer _broadcast;

    //! is recording
    bool _recording;
    //! is playing
    bool _playing;
    //! is there a broadcast waiting to be played
    bool _broadcastPending;

    void createAudioIO(QAudioFormat format);

};

#endif // AUDIOPLAYBACK_H
