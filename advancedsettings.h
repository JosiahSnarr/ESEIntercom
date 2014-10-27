#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <cstdint>

#include <QDialog>
#include <QJsonObject>

#include "serialcom.h"

#define FILE_ADVANCED_CONFIG "advanced_options.json"

#define USE_HEADER  "UseHeader"
#define ENCRYPTION_XOR  "EncryptionXOR"
#define COMPRESSION_HUFF "CompressionHuff"
#define COMPRESSION_RLE "CompressionRLE"

namespace Ui {
class AdvancedSettings;
}

/**
    Dialog for Advance settings
*/
class AdvancedSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = 0);
    ~AdvancedSettings();

    //! Settings
    struct Settings{
        bool useHeader;      ///< Send data in packets
        uint8_t bDecodeOpts; ///< Packet decode option
    };

    /**
        Get advanced options settings
    */
    Settings getSettings() const;

public slots:
    void onOkButtonClicked();
    void onSaveButtonClicked();

signals:

private:
    Ui::AdvancedSettings *ui;

    //! used to save and load configurations
    QJsonObject _json;

    //! the user settings
    Settings _settings;

    /**
        Update the settings
    */
    void updateSettings();
    /**
        Load settings from the file
    */
    void loadSettings();
    /**
        Save config to file
    */
    void saveSettings();

};

#endif // ADVANCEDSETTINGS_H
