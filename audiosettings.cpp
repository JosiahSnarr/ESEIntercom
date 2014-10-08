#include "audiosettings.h"
#include "ui_audiosettings.h"

#include <QDebug>

AudioSettings::AudioSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioSettings)
{
    ui->setupUi(this);

    // apply and cancel buttons
    connect(ui->bnOK, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
    connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(close()));

    fillParams();

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
    ui->cmbBitsPerSample->addItem("8");
    ui->cmbBitsPerSample->addItem("16");
}

AudioSettings::~AudioSettings()
{
    delete ui;
}
