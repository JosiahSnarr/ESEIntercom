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
    QAudioEncoderSettings settings;
    QJsonObject json;

    void fillParams();
    void updateSettings();

    void loadSettings();
};

#endif // AUDIOSETTINGS_H
