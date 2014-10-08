#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QDialog>

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

private:
    Ui::AudioSettings *ui;
    Settings settings;

    void fillParams();
    void updateSettings();
};

#endif // AUDIOSETTINGS_H
