
#include "phonebook.h"
#include <stdlib.h>

#include <QDebug>

static Sender* _newNode(Message* message, Sender* left, Sender* right)
{	
    Sender* sender = (Sender*)malloc(sizeof(Sender));

    if (sender == NULL) return NULL;

    sender->left = left;
    sender->right = right;

    sender->data.id = message->senderID;
    sender->data.timestamp = message->timestamp;
    sender->data.numMessages = 1;

    return sender;
}

static SenderData* _searchPhoneBook(Sender* sender, uint16_t key)
{
    if (sender == NULL) return NULL;

    if (key == sender->data.id)
        return &sender->data;
    else if (key < sender->data.id)
        return _searchPhoneBook(sender->left, key);
    else if (key > sender->data.id)
        return _searchPhoneBook(sender->right, key);

    return &sender->data;
}

static Sender* _insertIntoPhoneBook(Sender* sender, Message* message)
{
    if (sender == NULL) return _newNode(message, NULL, NULL);

    if (message->senderID == sender->data.id){
        sender->data.numMessages++;
        sender->data.timestamp = message->timestamp;
	}
    else if (message->senderID < sender->data.id)
        sender->left = _insertIntoPhoneBook(sender->left, message);
    else if (message->senderID > sender->data.id)
        sender->right = _insertIntoPhoneBook(sender->right, message);

    return sender;
}

static void _transversePhoneBookInOrder(Sender* sender, SenderFunction function)
{
    if (sender == NULL) return;

    _transversePhoneBookInOrder(sender->left, function);
    function(sender);
    _transversePhoneBookInOrder(sender->right, function);
}

void initPhoneBook(PhoneLog* phoneLog)
{
    phoneLog->root = NULL;
}

SenderData* searchPhoneBook(PhoneLog* log, uint16_t id)
{
    return _searchPhoneBook(log->root, id);
}

void insertIntoPhoneBook(PhoneLog* log, Message* message)
{
    log->root = _insertIntoPhoneBook(log->root, message);
}

void transversePhoneBookInOrder(PhoneLog* log, SenderFunction function)
{
    _transversePhoneBookInOrder(log->root, function);
}

void printPhoneLog(Sender* sender)
{
    qDebug() << "id: " << sender->data.id << ", messages: " << sender->data.numMessages << ", time: " << sender->data.timestamp;
}




