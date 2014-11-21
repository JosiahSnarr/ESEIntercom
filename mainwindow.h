#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "audiosettings.h"
#include "serialsettings.h"
#include "advancedsettings.h"

#include "serialcom.h"
#include "audioplayback.h"

#include "userlist.h"

namespace Ui {
class MainWindow;
}

/**
    Main UI
*/
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
    void onStreamButtonClicked();
    void onNextMessageButtonClicked();
    void onAddUserButtonClicked();
    void onUserListItemClicked();

    void newSession();
    void closeSession();

    void onMessageReceived(int numQueued);

    void onPlaybackStopped();
    void onStreamBufferSendReady(QByteArray&);

    void debugSerial();

private:
    Ui::MainWindow *ui;
    //! Audio settings dialog
    AudioSettings* audioSettings;
    //! serial settings dialog
    SerialSettings* serialSettings;
    //! advance options
    AdvancedSettings* advancedSettings;

    //! serial read write access
    SerialCom* serial;
    //! audio plack and recording
    AudioPlayback* audio;

    //! user list
    UserList userList;

    //!
    int senderId;
    int receiverId;

    //! this stations id
    int _id;

    /**
        Initialize the Menu Actions
    */
    void initMenuActions();

    /**
        Enable of Disable a group of ui components
    */
    void setEnabledUIComponents(bool enabled);

    /**
     * Updates the ui with info from a message
     *
     * @param message
     *      the message information
    */
    void updateMessageDisplay(Message* message);

    void showAlert(QString msg);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
