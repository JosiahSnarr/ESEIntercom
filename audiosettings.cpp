#include "audiosettings.h"
#include "ui_audiosettings.h"

#include <QFile>
#include <QJsonDocument>

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
    settings.nChannels = ui->cmbChannels->itemData(ui->cmbChannels->currentIndex()).toInt();
    settings.nBitsPerSample = ui->cmbBitsPerSample->itemData(ui->cmbBitsPerSample->currentIndex()).toInt();
    settings.nSamplesPerSec = ui->etSamplesPerSecond->text().toInt();
    settings.timeout = ui->etTimeout->text().toLong();
}

void AudioSettings::fillParams()
{
    // set channel options
    ui->cmbChannels->addItem("Mono", 1);
    ui->cmbChannels->addItem("Stereo", 2);

    // sampling options
    ui->cmbBitsPerSample->addItem("8 bits", 8);
    ui->cmbBitsPerSample->addItem("16 bits", 16);
}

void AudioSettings::loadSettings()
{
    QFile file("audio.json");
    if(file.exists()){

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QString content = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

        json = doc.object();

        settings.nChannels = json[NCHANNELS].toInt();
        settings.nBitsPerSample = json[NBITSPERSAMPLE].toInt();
        settings.nSamplesPerSec = json[NSAMPLESPERSECOND].toInt();
        settings.timeout = (long)json[LTIMEOUT].toInt();

        // update the dialoag to match
        ui->etTimeout->setText(QString::number(settings.timeout));
        ui->etSamplesPerSecond->setText(QString::number(settings.nSamplesPerSec));

        ui->cmbChannels->setCurrentIndex( (settings.nChannels == 1) ? 0 : 1 );
        ui->cmbBitsPerSample->setCurrentIndex( (settings.nBitsPerSample == 8) ? 0 : 1 );

        file.close();

    }
}

void AudioSettings::saveSettings()
{
    updateSettings();

    json[NCHANNELS] = settings.nChannels;
    json[NBITSPERSAMPLE] = settings.nBitsPerSample;
    json[NSAMPLESPERSECOND] = settings.nSamplesPerSec;
    json[LTIMEOUT] = settings.timeout;

    QJsonDocument doc(json);

    QFile file("audio.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.close();
}

AudioSettings::~AudioSettings()
{
    delete ui;
}
