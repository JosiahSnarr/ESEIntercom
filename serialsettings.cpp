#include "serialsettings.h"
#include "ui_serialsettings.h"

#include <QFile>
#include <QJsonDocument>

#include <QDebug>

SerialSettings::SerialSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialSettings)
{
    ui->setupUi(this);

    connect(ui->bnOK, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
    connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
    connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(close()));

    fillParams();
    loadSettings();
}

void SerialSettings::onOKButtonClicked()
{
    updateSettings();
    close();
}

void SerialSettings::onSaveButtonClicked()
{
    saveSettings();
}

void SerialSettings::loadSettings()
{
    QFile file(FILE_SERIAL_CONFIG);
    if(file.exists()){

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        // read json file
        QString content = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

        json = doc.object();

        // get settings from file
        settings.baud = json[BAUD].toInt();
        settings.parity = json[PARITY].toInt();
        settings.stopbits = json[STOPBITS].toInt();

        switch(settings.baud){
        case 9600:
            ui->cmbBaudRate->setCurrentIndex(0);
            break;
        case 11520:
            ui->cmbBaudRate->setCurrentIndex(1);
            break;
        }

        ui->cmbParity->setCurrentIndex(settings.parity);
        ui->cmbStopBits->setCurrentIndex(settings.stopbits);

    }
}

void SerialSettings::saveSettings()
{
    updateSettings();

    json[BAUD] = settings.baud;
    json[PARITY] = settings.parity;
    json[STOPBITS] = settings.stopbits;

    QFile file(FILE_SERIAL_CONFIG);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QJsonDocument doc(json);

    file.write(doc.toJson());
    file.close();
}

void SerialSettings::updateSettings()
{
    settings.baud = ui->cmbBaudRate->itemData(ui->cmbBaudRate->currentIndex()).toInt();
    settings.parity = ui->cmbParity->itemData(ui->cmbParity->currentIndex()).toInt();
    settings.stopbits = ui->cmbStopBits->itemData(ui->cmbStopBits->currentIndex()).toInt();
}

void SerialSettings::fillParams()
{
    ui->cmbBaudRate->addItem("9600", 9600);
    ui->cmbBaudRate->addItem("11520", 11520);

    ui->cmbParity->addItem("Even", 0);
    ui->cmbParity->addItem("Odd", 1);

    ui->cmbStopBits->addItem("0", 0);
    ui->cmbStopBits->addItem("1", 1);
}

SerialSettings::Settings SerialSettings::getSettings() const
{
    return settings;
}

SerialSettings::~SerialSettings()
{
    delete ui;
}
