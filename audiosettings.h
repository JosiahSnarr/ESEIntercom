#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QDialog>
#include <QJsonObject>
#include <QString>
#include <QAudioEncoderSettings>

#define NCHANNELS         "Channels"
#define NBITSPERSAMPLE    "BitsPerSample"
#define NSAMPLESPERSECOND "SamplesPerSecond"
#define LTIMEOUT          "Timeout"
#define AUDIOCODEC        "AudioCodec"

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

     QAudioEncoderSettings getSettings() const;

public slots:
    void onOKButtonClicked();
    void saveSettings();

private:
    Ui::AudioSettings *ui;
    //! holds settings
    QAudioEncoderSettings settings;
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
