/**
    @file audiosettings.cpp
    @breif Audio Settings
    @author Natesh Narain
*/

#include "audiosettings.h"
#include "ui_audiosettings.h"

#include <QFile>
#include <QJsonDocument>
#include <QAudioRecorder>

#include <QDebug>

#include "audiofilterbuffer.h"

AudioSettings::AudioSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioSettings)
{
    ui->setupUi(this);

    // apply and cancel buttons
    connect(ui->bnOK, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
    connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(close()));

    fillParams();
    loadSettings();

}

void AudioSettings::onOKButtonClicked()
{
    updateSettings();
    close();
}

void AudioSettings::updateSettings()
{
    settings.encoderSettings.setChannelCount(ui->cmbChannels->itemData(ui->cmbChannels->currentIndex()).toInt());
    settings.encoderSettings.setBitRate(ui->cmbBitsRate->itemData(ui->cmbBitsRate->currentIndex()).toInt());
    settings.encoderSettings.setSampleRate(ui->cmbSampleRate->itemData(ui->cmbSampleRate->currentIndex()).toInt());
    settings.encoderSettings.setCodec(ui->cmbAudioCodec->currentText());
    settings.upperThreshold = ui->cmbUpperThreshold->itemData(ui->cmbUpperThreshold->currentIndex()).toInt();
    settings.lowerThreshold = ui->cmbLowerThreshold->itemData(ui->cmbLowerThreshold->currentIndex()).toInt();
}

void AudioSettings::fillParams()
{
    QAudioRecorder info;

    // set channel options
    ui->cmbChannels->addItem("1", 1);
    ui->cmbChannels->addItem("2", 2);

    // sampling options
    foreach(const int sampleRate, info.supportedAudioSampleRates()){
        ui->cmbSampleRate->addItem(QString::number(sampleRate), sampleRate);
    }

    ui->cmbBitsRate->addItem("8000", 8000);
    ui->cmbBitsRate->addItem("16000", 16000);
    ui->cmbBitsRate->addItem("32000", 32000);
    ui->cmbBitsRate->addItem("64000", 64000);

    int i;
    for(i = 0; i <= AudioFilterBuffer::Amplitude::MAX; i++){
        ui->cmbUpperThreshold->addItem(QString::number(i), i);
        ui->cmbLowerThreshold->addItem(QString::number(i), i);
    }

    // audio codec options
    foreach(const QString codec, info.supportedAudioCodecs()){
        ui->cmbAudioCodec->addItem(codec);
    }
}

void AudioSettings::loadSettings()
{
    QFile file("audio.json");
    if(file.exists()){

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QString content = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

        json = doc.object();

        settings.encoderSettings.setChannelCount(json[NCHANNELS].toInt());
        settings.encoderSettings.setBitRate(json[NBITSPERSAMPLE].toInt());
        settings.encoderSettings.setSampleRate(json[NSAMPLESPERSECOND].toInt());
        settings.encoderSettings.setCodec(json[AUDIOCODEC].toString());

        settings.upperThreshold = json[UPPERTHRESHOLD].toInt();
        settings.lowerThreshold = json[LOWERTHRESHOLD].toInt();

        // update the dialoag to match
        ui->cmbSampleRate->setCurrentIndex(ui->cmbSampleRate->findData(settings.encoderSettings.sampleRate()));

        ui->cmbChannels->setCurrentIndex( ui->cmbChannels->findData(settings.encoderSettings.channelCount()) );
        ui->cmbBitsRate->setCurrentIndex( ui->cmbBitsRate->findData(settings.encoderSettings.bitRate()) );

        ui->cmbUpperThreshold->setCurrentIndex(ui->cmbUpperThreshold->findData(settings.upperThreshold));
        ui->cmbLowerThreshold->setCurrentIndex(ui->cmbLowerThreshold->findData(settings.lowerThreshold));

        file.close();

    }
}

void AudioSettings::saveSettings()
{
    updateSettings();

    json[NCHANNELS] = settings.encoderSettings.channelCount();
    json[NBITSPERSAMPLE] = settings.encoderSettings.bitRate();
    json[NSAMPLESPERSECOND] = settings.encoderSettings.sampleRate();
    json[AUDIOCODEC] = settings.encoderSettings.codec();

    json[UPPERTHRESHOLD] = settings.upperThreshold;
    json[LOWERTHRESHOLD] = settings.lowerThreshold;

    QJsonDocument doc(json);

    QFile file("audio.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.close();
}

AudioSettings::Settings AudioSettings::getSettings() const
{
    return settings;
}

AudioSettings::~AudioSettings()
{
    delete ui;
}
