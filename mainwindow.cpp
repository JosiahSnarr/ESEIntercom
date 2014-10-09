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

    // init serial com
    serial = new SerialCom(this);

    // connect button click events to respective slots
    connect(ui->bnRecord, SIGNAL(clicked()), this, SLOT(onRecordButtonClicked()));
    connect(ui->bnListen, SIGNAL(clicked()), this, SLOT(onListenButtonClicked()));
    connect(ui->bnSendAudio, SIGNAL(clicked()), this, SLOT(onSendAudioButtonClicked()));

    connect(ui->bnSendText, SIGNAL(clicked()), this, SLOT(onSendTextButtonClicked()));

    initMenuActions();
}

void MainWindow::onRecordButtonClicked()
{
    qDebug() << "Record Button Pressed\n";
}

void MainWindow::onListenButtonClicked()
{
    qDebug() << "Listen Button Pressed\n";
}

void MainWindow::onSendAudioButtonClicked()
{
    qDebug() << "Send Audio Button Pressed\n";
}

void MainWindow::onSendTextButtonClicked()
{
    qDebug() << "Send Text Button Pressed\n";
    QByteArray data("Hello World");
    serial->write(data);

}

void MainWindow::newSession()
{
    if(serial->open(serialSettings->getSettings())){
        ui->actionNew_Session->setEnabled(false);
        ui->actionClose_Session->setEnabled(true);
    }
    else{
        qDebug() << "Failed to open serial port" << "\n";
    }
}

void MainWindow::closeSession()
{
    serial->close();
    ui->actionNew_Session->setEnabled(true);
    ui->actionClose_Session->setEnabled(false);
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
}

MainWindow::~MainWindow()
{
    delete serial;
    delete audioSettings;
    delete serialSettings;
    delete ui;
}
