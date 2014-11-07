#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <cstdint>

#include <QDialog>
#include <QJsonObject>
#include <QString>
#include <QAudioEncoderSettings>

#define NCHANNELS         "Channels"
#define NBITSPERSAMPLE    "BitsPerSample"
#define NSAMPLESPERSECOND "SamplesPerSecond"
#define LTIMEOUT          "Timeout"
#define AUDIOCODEC        "AudioCodec"
#define UPPERTHRESHOLD    "UpperThreshold"
#define LOWERTHRESHOLD    "LowerThreshold"

namespace Ui {
class AudioSettings;
}

/**
    Dialog for Audio Settings
*/
class AudioSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettings(QWidget *parent = 0);
    ~AudioSettings();

    //! Audio Format settings (bit\sample rate, thresholds, etc)
    struct Settings{
        QAudioEncoderSettings encoderSettings; ///< audio format
        uint8_t upperThreshold;                ///< upper filter cut off
        uint8_t lowerThreshold;                ///< lower filter cut off
    };

     Settings getSettings() const;

public slots:
    void onOKButtonClicked();
    void saveSettings();

private:
    Ui::AudioSettings *ui;
    //! holds settings
    Settings settings;
    //! used for loading and write settings to a file
    QJsonObject json;

    /**
        Fill the dialog with options
    */
    void fillParams();

    /**
        Update the settings with user optios
    */
    void updateSettings();

    /**
        Load settings from a file
    */
    void loadSettings();
};

#endif // AUDIOSETTINGS_H
