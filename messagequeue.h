
/**
	Message Queue

	Define a FIFO queue for managing a set of messages

	@author Natesh Narain
*/

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_MAX 140 ///< Maximum length of a buffer

#define MESSAGE_DELIMITER "%%"

#define SUCCESS 0 ///< :)
#define FAILURE 1 ///< :(

#ifdef __cplusplus
extern "C"{
#endif

//! Structure of the message in the queue
typedef struct{
	char msg[140];         ///< message data. Same size as a tweet...
	uint16_t senderID;     ///< ID of the message sender
	uint16_t receiverID;   ///< ID of the message receiver
	uint8_t priority;      ///< Message Priority in the queue
	uint16_t msgSeq;       ///< Message Sequence
	uint8_t fun_bytes[25]; ///< Extra fun
}Message;

//! Linked List of Messages
typedef struct messageNode{
	struct messageNode* next; ///< Next message
	Message* msg;			  ///< Pointer to Message payload
}MessageNode;

//! FIFO queue containing the head and tail of the corresponding linked list
typedef struct {
	MessageNode* head; ///< The next node in the queue
	MessageNode* tail; ///< The last node in the queue
	int size;          ///< The number of messages in the queue
}MessageQueue;

/**
	Get a random message from the fortune file

	@param szBuf
		buffer to hold the message

	@param iLen
		Max characters to copy to the buffer
*/
int getMessageFromFile(char szBuf[], int iLen);

/**
	Initialize the Queue

	@param queue
		The queue to initialize
*/
void initQueue(MessageQueue* queue);

/**
	Add to the Queue

	@param queue
		The queue to add the message to

	@param msg
		The message to add to the queue
*/
int enQueue(MessageQueue* queue, Message* msg);

/**
	Get next message from the queue

	@param queue
		The queue
*/
Message* deQueue(MessageQueue* queue);

/**
	Check if the given queue is empty

	@param queue
		The given queue
*/
int isQueueEmpty(MessageQueue* queue);

#endif

#ifdef __cplusplus
}
#endif
