
/**
    @file mainwindow.cpp
    @breif Main user interface
    @author Natesh Narain
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QByteArray>
#include <QDateTime>

#include "bitopts.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // settings dialogs
    audioSettings = new AudioSettings(this);
    serialSettings = new SerialSettings(this);
    advancedSettings = new AdvancedSettings(this);

    // audio recording and playback
    audio = new AudioPlayback(audioSettings->getSettings(), this);
    connect(audio, SIGNAL(stoppedPlaying()), this, SLOT(onPlaybackStopped()));
    connect(audio, SIGNAL(onStreamBufferSendReady(QByteArray&)), this, SLOT(onStreamBufferSendReady(QByteArray&)));

    // init serial com
    serial = new SerialCom(this);
    connect(serial, SIGNAL(onQueueUpdate(int)), this, SLOT(onMessageReceived(int)));

    // connect serial com to audio broadcast player
    connect(serial, SIGNAL(onAudioReceived(QByteArray&)), audio, SLOT(onAudioReceived(QByteArray&)));
    connect(serial, SIGNAL(onAudioStreamReceived(QByteArray&)), audio, SLOT(onAudioStreamReceived(QByteArray&)));

    // connect button click events to respective slots
    connect(ui->bnRecord, SIGNAL(clicked()), this, SLOT(onRecordButtonClicked()));
    connect(ui->bnListen, SIGNAL(clicked()), this, SLOT(onListenButtonClicked()));
    connect(ui->bnSendAudio, SIGNAL(clicked()), this, SLOT(onSendAudioButtonClicked()));
    connect(ui->bnNextMessage, SIGNAL(clicked()), this, SLOT(onNextMessageButtonClicked()));
    connect(ui->bnStream, SIGNAL(clicked()), this, SLOT(onStreamButtonClicked()));

    connect(ui->bnSendText, SIGNAL(clicked()), this, SLOT(onSendTextButtonClicked()));
    ui->bnNextMessage->setEnabled(false);

    initMenuActions();

    // disable components
    setEnabledUIComponents(false);

    ui->statusBar->showMessage("Ready");
}

void MainWindow::onRecordButtonClicked()
{
    if(audio->isRecording()){
        audio->stopRecording();
        ui->bnRecord->setText("Record");
    }
    else{
        audio->record();
        ui->bnRecord->setText("Stop Recording");
    }
}

void MainWindow::onListenButtonClicked()
{
    if(audio->isPlaying()){
        audio->stopPlayback();
        ui->bnListen->setText("Listen");
    }
    else if(!audio->isPlaying() && !audio->isRecording()){
        audio->play();
        ui->bnListen->setText("Stop Listening");
    }
}

void MainWindow::onPlaybackStopped()
{
    ui->bnListen->setText("Listen");
}

void MainWindow::onSendAudioButtonClicked()
{
    QBuffer buffer;
    audio->getRecordedAudio(buffer);

    QByteArray data = buffer.data();

    AdvancedSettings::Settings settings = advancedSettings->getSettings();
    uint8_t decodeOptions = settings.bDecodeOpts;
    setbit(decodeOptions, MSG_TYPE_AUDIO);

    serial->write(data, 99, settings.useHeader, decodeOptions);
}

void MainWindow::onStreamButtonClicked()
{
    audio->startStreamingRecording();
}

void MainWindow::onStreamBufferSendReady(QByteArray& buffer)
{
    qDebug() << "Sending audio stream";

    AdvancedSettings::Settings settings = advancedSettings->getSettings();
    setbit(settings.bDecodeOpts, MSG_TYPE_AUDIO_STREAM);

    serial->write(buffer, 99, settings.useHeader, settings.bDecodeOpts);
}

void MainWindow::onSendTextButtonClicked()
{
    QString content = ui->etSend->toPlainText();

    QByteArray data;
    data.append(content);

    qDebug() << "content size: " << content.length() << "\n";

    AdvancedSettings::Settings settings = advancedSettings->getSettings();
    uint8_t decodeOpts = settings.bDecodeOpts;
    setbit(decodeOpts, MSG_TYPE_TEXT);

    serial->write(data, 99, settings.useHeader, decodeOpts);

    ui->etSend->setPlainText("");
}


void MainWindow::onPhoneBookButtonClicked()
{
    serial->printPhoneBook();
}

void MainWindow::onNextMessageButtonClicked()
{
    Message* message = serial->getNextMessageFromQueue();
    updateMessageDisplay(message);
    free(message);
}

void MainWindow::updateMessageDisplay(Message* message)
{
    // put the message in the display
    QString displayText(message->msg);
    ui->etReceived->setPlainText(displayText);

    // set the user id
    ui->lbSenderId->setText(QString("From: %1").arg(message->senderID));

    // get the timestamp
    QDateTime date = QDateTime::fromTime_t(message->timestamp).toUTC();

    QString timeStampLabel("Time: ");
    QString timeStamp = date.toString("h:m:s ap");
    timeStampLabel.append(timeStamp);
    ui->lbTimeStamp->setText(timeStampLabel);

}

void MainWindow::onMessageReceived(int numQueued)
{
    ui->lbNumQueued->setText(QString("Messages: %1").arg(numQueued));

    if(numQueued == 0)
        ui->bnNextMessage->setEnabled(false);
    else if(numQueued > 0)
        ui->bnNextMessage->setEnabled(true);
}

void MainWindow::newSession()
{
    SerialSettings::Settings settings = serialSettings->getSettings();
    AdvancedSettings::Settings advancedSetting = advancedSettings->getSettings();

    serial->setUseHeader(advancedSetting.useHeader);

    if(serial->open(settings)){
        ui->actionNew_Session->setEnabled(false);
        ui->actionClose_Session->setEnabled(true);
        setEnabledUIComponents(true);
        ui->statusBar->showMessage("Serial Communication Opened: " + settings.portName);
    }
    else{
        ui->statusBar->showMessage("Failed to open serial port");
        qDebug() << "Failed to open serial port" << "\n";
    }
}

void MainWindow::closeSession()
{
    serial->close();
    ui->actionNew_Session->setEnabled(true);
    ui->actionClose_Session->setEnabled(false);
    setEnabledUIComponents(false);

    ui->statusBar->showMessage("Serial Communication closed");
}

/**
    Debug Serial
*/
void MainWindow::debugSerial()
{
    char str[BUFFER_MAX];
    getMessageFromFile(str, BUFFER_MAX);

    QString debugMessage(str);
    QByteArray data;
    data.append(debugMessage);

    AdvancedSettings::Settings settings = advancedSettings->getSettings();
    uint8_t decodeOpts = settings.bDecodeOpts;
    setbit(decodeOpts, MSG_TYPE_TEXT);

    serial->write(data, 99, settings.useHeader, decodeOpts);
}

void MainWindow::initMenuActions()
{
    // connect session actions
    connect(ui->actionNew_Session, SIGNAL(triggered()), this, SLOT(newSession()));
    connect(ui->actionClose_Session, SIGNAL(triggered()), this, SLOT(closeSession()));

    ui->actionClose_Session->setEnabled(false);

    // connect cofiguration dialogs
    connect(ui->actionAudio_Settings, SIGNAL(triggered()), audioSettings, SLOT(show()));
    connect(ui->actionSerial_Settings, SIGNAL(triggered()), serialSettings, SLOT(show()));
    connect(ui->actionAdvancedOptions, SIGNAL(triggered()), advancedSettings, SLOT(show()));

    // connect the debug serial o/p
    connect(ui->actionDebugSerial, SIGNAL(triggered()), this, SLOT(debugSerial()));
}

void MainWindow::setEnabledUIComponents(bool enabled)
{
    ui->bnSendText->setEnabled(enabled);
    ui->bnRecord->setEnabled(enabled);
    ui->bnSendAudio->setEnabled(enabled);
    ui->bnListen->setEnabled(enabled);
    ui->bnStream->setEnabled(enabled);
}

MainWindow::~MainWindow()
{
    delete serial;
    delete audio;
    delete audioSettings;
    delete serialSettings;
    delete ui;
}
