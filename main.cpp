

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "playback.h"
#include "messagequeue.h"

#include "config.h"

#include "mainwindow.h"
#include <QApplication>

#define KILOBYTE 1024

// global vars from config file
WORD nChannels;
DWORD nSamplesPerSec;
WORD nBitsPerSample;
WORD cbSize;
long timeout;

int main(int argc, char *argv[])
{
   // QApplication a(argc, argv);
   // MainWindow w;
   //w.show();

    // load global config vars
    configInit("config.lua");
    nChannels      = (WORD) configInt("nChannels");
    nSamplesPerSec = (DWORD)configInt("nSamplesPerSec");
    nBitsPerSample = (WORD) configInt("wBitsPerSample");
    cbSize         = (WORD) configInt("cbSize");
    timeout        = (long) configInt("timeout");

    // allocate space for audio data
    long size = (((nBitsPerSample / 8) * nSamplesPerSec) * timeout * nChannels) / 2;
    short * audioBuffer = (short *)malloc(size * sizeof(short));

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

    //return a.exec();
    return 0;
}
