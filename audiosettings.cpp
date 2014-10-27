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
    settings.setChannelCount(ui->cmbChannels->itemData(ui->cmbChannels->currentIndex()).toInt());
    settings.setBitRate(ui->cmbBitsRate->itemData(ui->cmbBitsRate->currentIndex()).toInt());
    settings.setSampleRate(ui->cmbSampleRate->itemData(ui->cmbSampleRate->currentIndex()).toInt());
   // settings.timeout = ui->etTimeout->text().toLong();
    settings.setCodec(ui->cmbAudioCodec->currentText());
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

        settings.setChannelCount(json[NCHANNELS].toInt());
        settings.setBitRate(json[NBITSPERSAMPLE].toInt());
        settings.setSampleRate(json[NSAMPLESPERSECOND].toInt());
        //settings.timeout = (long)json[LTIMEOUT].toInt();
        settings.setCodec(json[AUDIOCODEC].toString());

        // update the dialoag to match
       // ui->etTimeout->setText(QString::number(settings.timeout));
        ui->cmbSampleRate->setCurrentIndex(ui->cmbSampleRate->findData(settings.sampleRate()));

        ui->cmbChannels->setCurrentIndex( ui->cmbChannels->findData(settings.channelCount()) );
        ui->cmbBitsRate->setCurrentIndex( ui->cmbBitsRate->findData(settings.bitRate()) );

        file.close();

    }
}

void AudioSettings::saveSettings()
{
    updateSettings();

    json[NCHANNELS] = settings.channelCount();
    json[NBITSPERSAMPLE] = settings.bitRate();
    json[NSAMPLESPERSECOND] = settings.sampleRate();
    //json[LTIMEOUT] = settings.timeout;
    json[AUDIOCODEC] = settings.codec();

    QJsonDocument doc(json);

    QFile file("audio.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.close();
}

QAudioEncoderSettings AudioSettings::getSettings() const
{
    return settings;
}

AudioSettings::~AudioSettings()
{
    delete ui;
}
