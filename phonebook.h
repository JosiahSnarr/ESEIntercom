

#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdint.h>
#include <string.h>

#include "messagequeue.h"

#define STR_MAX 140

#ifdef __cplusplus
extern "C"{
#endif

typedef struct senderData{
	int id;
	int numMessages;
	long timestamp;
}SenderData;

typedef struct userNode{
	struct userNode* left;
	struct userNode* right;
    SenderData data;
}Sender;

typedef struct phoneLog{
    Sender* root;
}PhoneLog;

typedef void(*SenderFunction)(Sender*);

void initPhoneBook(PhoneLog* log);
SenderData* searchPhoneBook(PhoneLog* log, uint16_t id);

void insertIntoPhoneBook(PhoneLog* log, Message* message);

void transversePhoneBookInOrder(PhoneLog* log, SenderFunction function);

#ifdef __cplusplus
}
#endif

#endif
