

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

#define BITS_TO_BYTES(b) (b / 8)
#define TIMEMS_TO_SEC(t) (t / 1000)

// global vars from config file
WORD nChannels;
DWORD nSamplesPerSec;
WORD nBitsPerSample;
WORD cbSize;
long timeout;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // load global config vars
    int ret = configInit("config.lua");

    if(ret){
        printf("configuration file failed to load\n");
        return 1;
    }

    nChannels      = (WORD) configInt("nChannels");
    nSamplesPerSec = (DWORD)configInt("nSamplesPerSec");
    nBitsPerSample = (WORD) configInt("wBitsPerSample");
    cbSize         = (WORD) configInt("cbSize");
    timeout        = (long) configInt("timeout");

    // allocate space for audio data
    long size = ((BITS_TO_BYTES(nBitsPerSample) * nSamplesPerSec) * TIMEMS_TO_SEC(timeout) * nChannels) / 2;
    short * audioBuffer = (short *)malloc(size * sizeof(short));

    qDebug() << size << "\n";

    // record audio
    if (InitializeRecording())
    {
        RecordBuffer(audioBuffer, size);
        CloseRecording();

        // play buffer
        if (InitializePlayback())
        {
            PlayBuffer(audioBuffer, size);
            ClosePlayback();
        }
    }

    free(audioBuffer);

    return a.exec();
    //return 0;
}
