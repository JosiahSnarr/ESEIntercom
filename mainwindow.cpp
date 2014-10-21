#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // settings dialogs
    audioSettings = new AudioSettings(this);
    serialSettings = new SerialSettings(this);

    // audio recording and playback
    audio = new AudioPlayback(audioSettings->getSettings(), this);

    connect(audio, SIGNAL(stoppedPlaying()), this, SLOT(onPlaybackStopped()));

    // init serial com
    serial = new SerialCom(this);
    connect(serial, SIGNAL(messageReceived(QString)), this, SLOT(updateMessages(QString)));

    // connect button click events to respective slots
    connect(ui->bnRecord, SIGNAL(clicked()), this, SLOT(onRecordButtonClicked()));
    connect(ui->bnListen, SIGNAL(clicked()), this, SLOT(onListenButtonClicked()));
    connect(ui->bnSendAudio, SIGNAL(clicked()), this, SLOT(onSendAudioButtonClicked()));

    connect(ui->bnSendText, SIGNAL(clicked()), this, SLOT(onSendTextButtonClicked()));

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
    audio->stopPlayback();
    ui->bnListen->setText("Listen");
}

void MainWindow::onSendAudioButtonClicked()
{
    qDebug() << "Send Audio Button Pressed\n";
}

void MainWindow::onSendTextButtonClicked()
{
    qDebug() << "Send Text Button Pressed\n";

    QString content = ui->etSend->toPlainText();
    QByteArray data;
    data.append(content);

    serial->write(data, 0);
}

void MainWindow::updateMessages(QString msg)
{
    ui->etReceived->appendPlainText(msg + "\n");
}

void MainWindow::newSession()
{
    SerialSettings::Settings settings = serialSettings->getSettings();
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
    QByteArray data;
    data.append(DEBUG_SERIAL_OUT);
    serial->write(data, 0);
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

    // connect the debug serial o/p
    connect(ui->actionDebugSerial, SIGNAL(triggered()), this, SLOT(debugSerial()));
}

void MainWindow::setEnabledUIComponents(bool enabled)
{
    ui->bnSendText->setEnabled(enabled);
    ui->bnRecord->setEnabled(enabled);
    ui->bnSendAudio->setEnabled(enabled);
    ui->bnListen->setEnabled(enabled);
}

MainWindow::~MainWindow()
{
    delete serial;
    delete audio;
    delete audioSettings;
    delete serialSettings;
    delete ui;
}
