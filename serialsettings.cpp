#include "serialsettings.h"
#include "ui_serialsettings.h"

#include <QFile>
#include <QJsonDocument>
#include <QtSerialPort/QSerialPortInfo>

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

        settings.portName = json[PORTNAME].toString();
        settings.baudrate = static_cast<QSerialPort::BaudRate>(json[BAUD].toInt());
        settings.databits = static_cast<QSerialPort::DataBits>(json[DATABITS].toInt());
        settings.stopbits = static_cast<QSerialPort::StopBits>(json[STOPBITS].toInt());
        settings.parity   = static_cast<QSerialPort::Parity>(json[PARITY].toInt());
        settings.flowcontrol = static_cast<QSerialPort::FlowControl>(json[FLOWCONTROL].toInt());

        ui->cmbPortName->setCurrentText(settings.portName);
        ui->cmbBaudRate->setCurrentIndex(ui->cmbBaudRate->findData(settings.baudrate));
        ui->cmbDataBits->setCurrentIndex(ui->cmbDataBits->findData(settings.databits));
        ui->cmbParity->setCurrentIndex(ui->cmbParity->findData(settings.parity));
        ui->cmbStopBits->setCurrentIndex(ui->cmbStopBits->findData(settings.stopbits));
        ui->cmbFlowControl->setCurrentIndex(ui->cmbFlowControl->findData(settings.flowcontrol));

    }
}

void SerialSettings::saveSettings()
{
    updateSettings();

    json[PORTNAME] = settings.portName;
    json[BAUD] = settings.baudrate;
    json[PARITY] = settings.parity;
    json[STOPBITS] = settings.stopbits;
    json[DATABITS] = settings.databits;
    json[FLOWCONTROL] = settings.flowcontrol;

    QFile file(FILE_SERIAL_CONFIG);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QJsonDocument doc(json);

    file.write(doc.toJson());
    file.close();
}

void SerialSettings::updateSettings()
{
    settings.portName = ui->cmbPortName->currentText();
    settings.baudrate = static_cast<QSerialPort::BaudRate>(ui->cmbBaudRate->itemData(ui->cmbBaudRate->currentIndex()).toInt());
    settings.parity = static_cast<QSerialPort::Parity>(ui->cmbParity->itemData(ui->cmbParity->currentIndex()).toInt());
    settings.stopbits = static_cast<QSerialPort::StopBits>(ui->cmbStopBits->itemData(ui->cmbStopBits->currentIndex()).toInt());
    settings.databits = static_cast<QSerialPort::DataBits>(ui->cmbDataBits->itemData(ui->cmbDataBits->currentIndex()).toInt());
    settings.flowcontrol = static_cast<QSerialPort::FlowControl>(ui->cmbFlowControl->itemData(ui->cmbFlowControl->currentIndex()).toInt());
}

void SerialSettings::fillParams()
{
    // fill baud rate options
    ui->cmbBaudRate->addItem("1200", QSerialPort::Baud1200);
    ui->cmbBaudRate->addItem("2400", QSerialPort::Baud2400);
    ui->cmbBaudRate->addItem("4800", QSerialPort::Baud4800);
    ui->cmbBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->cmbBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->cmbBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->cmbBaudRate->addItem("57600", QSerialPort::Baud57600);
    ui->cmbBaudRate->addItem("115200", QSerialPort::Baud115200);

    // fill parity option
    ui->cmbParity->addItem("None", QSerialPort::NoParity);
    ui->cmbParity->addItem("Even", QSerialPort::EvenParity);
    ui->cmbParity->addItem("Odd", QSerialPort::OddParity);

    // fill stop bit options
    ui->cmbStopBits->addItem("1", QSerialPort::OneStop);
    ui->cmbStopBits->addItem("2", QSerialPort::TwoStop);

    // fill data bit option
    ui->cmbDataBits->addItem("5", QSerialPort::Data5);
    ui->cmbDataBits->addItem("6", QSerialPort::Data6);
    ui->cmbDataBits->addItem("7", QSerialPort::Data7);
    ui->cmbDataBits->addItem("8", QSerialPort::Data8);

    // fill flow control option
    ui->cmbFlowControl->addItem("None", QSerialPort::NoFlowControl);
    ui->cmbFlowControl->addItem("Software", QSerialPort::SoftwareControl);
    ui->cmbFlowControl->addItem("Hardware", QSerialPort::HardwareControl);

    // fill port names
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts()){
        ui->cmbPortName->addItem(info.portName());
    }
}

SerialSettings::Settings SerialSettings::getSettings() const
{
    return settings;
}

SerialSettings::~SerialSettings()
{
    delete ui;
}
