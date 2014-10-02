#include "mainwindow.h"
#include <QApplication>

#include "messagequeue.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    MessageQueue queue;
    initQueue(&queue);

    int N = 10;
    int i;

    char str[32] = "Message #";

    for(i = 0; i < N; ++i)
    {
        Message * message = (Message *) malloc(sizeof(Message));
        sprintf(str, "%d", i);
        strcpy(message->msg, str);
        enQueue(&queue, message);
    }

    // print out the messages
    const int size = queue.size;

    for(i = 0; i < size; i++)
    {
        Message * message = deQueue(&queue);
        printf("%s\n", message->msg);

        free(message);
    }

    return a.exec();
}
