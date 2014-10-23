#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "audiosettings.h"
#include "serialsettings.h"

#include "serialcom.h"
#include "audioplayback.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onRecordButtonClicked();
    void onListenButtonClicked();
    void onSendAudioButtonClicked();
    void onSendTextButtonClicked();
    void onNextMessageButtonClicked();

    void newSession();
    void closeSession();

    void onMessageReceived(int numQueued);

    void onPlaybackStopped();

    void debugSerial();

private:
    Ui::MainWindow *ui;
    AudioSettings* audioSettings;
    SerialSettings* serialSettings;

    SerialCom* serial;
    AudioPlayback* audio;

    void initMenuActions();
    void setEnabledUIComponents(bool enabled);
    void updateMessageDisplay(Message* message);
};

#endif // MAINWINDOW_H
