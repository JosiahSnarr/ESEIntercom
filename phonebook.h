

#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdint.h>
#include <string.h>

#include "messagequeue.h"

#define STR_MAX 140

#ifdef __cplusplus
extern "C"{
#endif

//! Information on the sender
typedef struct senderData{
    int id;          ///< The id of the sender
    int numMessages; ///< The number of messages from that sender
    uint32_t timestamp;  ///< The data of the last message from the sender
}SenderData;

//! Sender node
typedef struct senderNode{
    struct senderNode* left;  ///< next node to the left
    struct senderNode* right; ///< next node to the right
    SenderData data;          ///< the data of the Sender
}Sender;

//! Phone log tree
typedef struct phoneLog{
    Sender* root; ///< root node
}PhoneLog;

//! Funtion pointer type to apply to sender data
typedef void(*SenderFunction)(Sender*);

/**
    Initialize the phone book

    @param log
        PhoneLog
*/
void initPhoneBook(PhoneLog* log);

/**
    Search the phone book for a user

    @param log
        The phone log

    @param id
        Sender id

    @return sender data at the id
*/
SenderData* searchPhoneBook(PhoneLog* log, uint16_t id);

/**
    Insert a message into  the phone log

    @param log
        the phone log

    @param message
        The message to add
*/
void insertIntoPhoneBook(PhoneLog* log, Message* message);

/**
    In order transversal of the tree

    @param log
        The phone log

    @param the function to apply to all nodes in order
*/
void transversePhoneBookInOrder(PhoneLog* log, SenderFunction function);

void printPhoneLog(Sender* data);

#ifdef __cplusplus
}
#endif

#endif
