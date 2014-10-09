#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

#include "serialsettings.h"
#include "messagequeue.h"

#define READY_READ_SIZE 11

class SerialCom : public QObject
{
    Q_OBJECT
public:
    explicit SerialCom(QObject *parent = 0);
    ~SerialCom();

signals:

public slots:
    void onDataReceived();

public:
    bool open(SerialSettings::Settings settings);
    void close();
    void write(QByteArray data);

private:
    QSerialPort* serial;

};

#endif // SERIALCOM_H
