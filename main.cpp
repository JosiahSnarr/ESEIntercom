

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "playback.h"
#include "messagequeue.h"

#include "config.h"

#include "mainwindow.h"
#include <QApplication>
#include <QString>

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

    // load global config vars
    int ret = configInit("config.lua");

    if(ret){
        printf("configuration file failed to load\n%s\n", configError());
        return 1;
    }

    g_nChannels        = (WORD)  configInt("nChannels");
    g_nSamplesPerSec   = (DWORD) configInt("nSamplesPerSec");
    g_nBitsPerSample   = (WORD)  configInt("wBitsPerSample");
    g_cbSize           = (WORD)  configInt("cbSize");
    g_lTimeout         = (long)  configInt("timeout");
    g_wAudioBufferSize = (WORD)  configInt("AudioBufferSize");

    const char * windowName = configString("AppName");

    w.setWindowTitle(QString(windowName));

    // open the main window and start the event loop
    w.show();
    return a.exec();
}
