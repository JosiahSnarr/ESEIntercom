
/**
    @file main.cpp
    @breif Where the magic starts
    @author Natesh Narain
*/

#include <time.h>

#include "mainwindow.h"
#include <QApplication>

#include "messagequeue.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
