#include "advancedsettings.h"
#include "ui_advancedsettings.h"

AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    connect(ui->bnOK, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
    connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
    connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui->cbCompressNone, SIGNAL(clicked()), this, SLOT(onCompressNoneCheckboxChecked()));
    connect(ui->cbEncryptNone, SIGNAL(clicked()), this, SLOT(onEncryptNoneCheckboxChecked()));
    connect(ui->cbCompressHuff, SIGNAL(clicked()), this, SLOT(onCompressCheckboxChecked()));
    connect(ui->cbCompressRLE, SIGNAL(clicked()), this, SLOT(onCompressCheckboxChecked()));
    connect(ui->cbEncryptXOR, SIGNAL(clicked()), this, SLOT(onEncryptCheckboxChecked()));

    ui->cbCompressRLE->setChecked(true);
    ui->cbEncryptNone->setChecked(true);
    ui->rbPacketFrame->setChecked(true);
}

void AdvancedSettings::onOkButtonClicked()
{

}

void AdvancedSettings::onSaveButtonClicked()
{

}

void AdvancedSettings::onCompressCheckboxChecked()
{
    ui->cbCompressNone->setChecked(false);
}

void AdvancedSettings::onEncryptCheckboxChecked()
{
    ui->cbEncryptNone->setChecked(false);
}

void AdvancedSettings::onCompressNoneCheckboxChecked()
{
    ui->cbCompressHuff->setChecked(false);
    ui->cbCompressRLE->setChecked(false);
    ui->cbCompressNone->setChecked(true);
}

void AdvancedSettings::onEncryptNoneCheckboxChecked()
{
    ui->cbEncryptXOR->setChecked(false);
    ui->cbEncryptNone->setChecked(true);
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}
