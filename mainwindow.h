#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "audiosettings.h"
#include "serialsettings.h"

#include "serialcom.h"
#include "audiorecorder.h"

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

    void newSession();
    void closeSession();

    void updateMessages(QString);

private:
    Ui::MainWindow *ui;
    AudioSettings* audioSettings;
    SerialSettings* serialSettings;

    SerialCom* serial;
    AudioRecorder* audio;

    void initMenuActions();
    void setEnabledUIComponents(bool enabled);
};

#endif // MAINWINDOW_H
