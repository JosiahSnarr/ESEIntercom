#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QDialog>
#include <QJsonObject>

#define NCHANNELS "Channels"
#define NBITSPERSAMPLE "BitsPerSample"
#define NSAMPLESPERSECOND "SamplesPerSecond"
#define LTIMEOUT "Timeout"

namespace Ui {
class AudioSettings;
}

class AudioSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettings(QWidget *parent = 0);
    ~AudioSettings();

    struct Settings{
        int nChannels;
        int nSamplesPerSec;
        int nBitsPerSample;
        long timeout;
    };

public slots:
    void onOKButtonClicked();
    void saveSettings();

private:
    Ui::AudioSettings *ui;
    Settings settings;
    QJsonObject json;

    void fillParams();
    void updateSettings();

    void loadSettings();
};

#endif // AUDIOSETTINGS_H
