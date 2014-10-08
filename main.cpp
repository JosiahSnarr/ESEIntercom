
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "playback.h"
#include "messagequeue.h"

#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QIcon>

#include <QDebug>

// global vars from config file
WORD  g_nChannels;
DWORD g_nSamplesPerSec;
WORD  g_nBitsPerSample;
WORD  g_cbSize;
long  g_lTimeout;
short g_wAudioBufferSize;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
