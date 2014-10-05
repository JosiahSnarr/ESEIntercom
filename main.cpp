

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "playback.h"
#include "messagequeue.h"

#include "config.h"

#include "mainwindow.h"
#include <QApplication>

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

    // allocate space for audio data
    short * audioBuffer = (short *)malloc(g_wAudioBufferSize * sizeof(short));

    // record audio
    if (InitializeRecording())
    {
        RecordBuffer(audioBuffer, g_wAudioBufferSize);
        CloseRecording();

        // play buffer
        if (InitializePlayback())
        {
            PlayBuffer(audioBuffer, g_wAudioBufferSize);
            ClosePlayback();
        }
    }

    free(audioBuffer);

    w.show();
    return a.exec();
    //return 0;
}
