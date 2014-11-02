#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include <QObject>
#include <QBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioEncoderSettings>
#include <QTimer>

#include "audiofilterbuffer.h"
#include "streambuffer.h"
#include "audiosettings.h"

/**
    Audio Recording, Playback and Broadcasts
*/
class AudioPlayback : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayback(AudioSettings::Settings format, QObject *parent = 0);
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
        Start an audio stream
    */
    void startStreamingRecording();

    /**
        Stop streaming
    */
    void stopStreamingRecording();

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
    void setAudioFormat(AudioSettings::Settings format);

    /**
        @return whether currently recording or not
    */
    bool isRecording() const;

    /**
        @return whether currently recording or not
    */
    bool isPlaying() const;

    /**
        @return whether currently streaming or not
    */
    bool isStreaming() const;

    /**
        Get the recorded audio

        @param buffer
            buffer to copy the audio into
    */
    void getRecordedAudio(QBuffer& buffer) const;

public slots:
    void onPlayerStateChanged(QAudio::State);
    void onAudioReceived(QByteArray&);
    void onAudioStreamReceived(QByteArray& buffer);
    void onTick();

signals:
    void stoppedPlaying();
    void onStreamBufferSendReady(QByteArray&);

private:
    //! recording
    QAudioInput*  _input;
    //! playing
    QAudioOutput* _output;
    //! buffer to hold recorded data
    AudioFilterBuffer _buffer;
    //! buffer to hold the audio stream
    StreamBuffer _streamBuffer;
    //! buffer used to hold broadcasted audio
    QBuffer _broadcast;

    //! Timer for streaming
    QTimer* _timer;

    //! is recording
    bool _recording;
    //! is playing
    bool _playing;
    //! is there a broadcast waiting to be played
    bool _broadcastPending;
    //! is streaming recording stream audio
    bool _isStreamRecording;
    //! is playing stream audio
    bool _isStreamPlaying;

    void createAudioIO(QAudioFormat format);

};

#endif // AUDIOPLAYBACK_H
