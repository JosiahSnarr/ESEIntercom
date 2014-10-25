#include "advancedsettings.h"
#include "ui_advancedsettings.h"

#include <QFile>
#include <QJsonDocument>
#include "bitopts.h"

AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    connect(ui->bnOK, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
    connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
    connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(close()));

    _settings.bDecodeOpts = 0;

    loadSettings();
}

void AdvancedSettings::onOkButtonClicked()
{
    updateSettings();
    close();
}

void AdvancedSettings::onSaveButtonClicked()
{
    saveSettings();
}

void AdvancedSettings::loadSettings()
{
    QFile file(FILE_ADVANCED_CONFIG);
    if(file.exists()){

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        // read the json file
        QString content = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

        _json = doc.object();

        // fill settings
        bool useHeader = _json[USE_HEADER].toBool();
        bool XOR = _json[ENCRYPTION_XOR].toBool();
        bool huff = _json[COMPRESSION_HUFF].toBool();
        bool rle = _json[COMPRESSION_RLE].toBool();

        if(useHeader){
            ui->rbPacketFrame->setChecked(true);
            _settings.useHeader = useHeader;
        }
        else{
            ui->rbPacketRaw->setChecked(true);
        }

        if(XOR){
            ui->cbEncryptXOR->setChecked(true);
            setbit(_settings.bDecodeOpts, ENCRYPT_TYPE_XOR);
        }

        if(huff){
            ui->cbCompressHuff->setChecked(true);
            setbit(_settings.bDecodeOpts, COMPRESS_TYPE_HUFF);
        }

        if(rle){
            ui->cbCompressRLE->setChecked(true);
            setbit(_settings.bDecodeOpts, COMPRESS_TYPE_RLE);
        }

        file.close();
    }
}

void AdvancedSettings::updateSettings()
{
    if(ui->cbCompressHuff->isChecked())
        setbit(_settings.bDecodeOpts, COMPRESS_TYPE_HUFF);
    else
        clearbit(_settings.bDecodeOpts, COMPRESS_TYPE_HUFF);

    if(ui->cbCompressRLE->isChecked())
        setbit(_settings.bDecodeOpts, COMPRESS_TYPE_RLE);
    else
        clearbit(_settings.bDecodeOpts, COMPRESS_TYPE_RLE);

    if(ui->cbEncryptXOR->isChecked())
        setbit(_settings.bDecodeOpts, ENCRYPT_TYPE_XOR);
    else
        clearbit(_settings.bDecodeOpts, ENCRYPT_TYPE_XOR);

    _settings.useHeader = ui->rbPacketFrame->isChecked();
}

void AdvancedSettings::saveSettings()
{
    updateSettings();

    _json[USE_HEADER] = _settings.useHeader;

    _json[COMPRESSION_HUFF] = (isBitSet(_settings.bDecodeOpts, COMPRESS_TYPE_HUFF)) ? true : false;
    _json[COMPRESSION_RLE] = (isBitSet(_settings.bDecodeOpts, COMPRESS_TYPE_RLE)) ? true : false;
    _json[ENCRYPTION_XOR] = (isBitSet(_settings.bDecodeOpts, ENCRYPT_TYPE_XOR)) ? true : false;

    QFile file(FILE_ADVANCED_CONFIG);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QJsonDocument doc(_json);

    file.write(doc.toJson());
    file.close();;
}

AdvancedSettings::Settings AdvancedSettings::getSettings() const
{
    return _settings;
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}
