

#include "messagequeue.h"

static void _printMessage(MessageNode* message);
static void _deleteNode(MessageNode* node);

void initQueue(MessageQueue* queue)
{
	queue->head = queue->tail = NULL;
	queue->size = 0;
}

int enQueue(MessageQueue* queue, Message* message)
{
	// create a new message node and add the message to it
	MessageNode * node = (MessageNode *)malloc(sizeof(MessageNode));

	if (node == NULL) return FAILURE;

	node->msg = message;
	node->next = NULL;

	// if the queue is empty add the node to the head
	// else add to the end of the tail
	if (isQueueEmpty(queue)){
		queue->head = node;
	}
	else{
		queue->tail->next = node;
	}

	// set the new node as the tail
	queue->tail = node;
	queue->size++;

	return SUCCESS;
}

Message* deQueue(MessageQueue* queue)
{
	Message* ret = NULL;
	MessageNode* node;

	// if the queue is empty return nothing
	if (isQueueEmpty(queue)) return NULL; // :(

	// get the current nodes message pointer
	node = queue->head;
	ret = node->msg;

	// set the head to the next node
	queue->head = queue->head->next;
	
	// release the previous node
	free(node);
	queue->size--;

	return ret;
}

int isQueueEmpty(MessageQueue* queue)
{
	return (queue->head == NULL);
}

void transverse(MessageNode* node, MessageNodeFunction function)
{
    if (node == NULL) return;
    function(node);
    transverse(node->next, function);
}

void transverseR(MessageNode* node, MessageNodeFunction function)
{
    if (node == NULL) return;
    transverseR(node->next, function);
    function(node);
}

void printMessages(MessageQueue* queue, BOOL reversed)
{
    if (reversed == FALSE){
        transverse(queue->head, _printMessage);
    }
    else{
        transverseR(queue->head, _printMessage);
    }
}

static void _printMessage(MessageNode* node)
{
    printf("\n%s\n", node->msg->msg);
}

void deleteQueue(MessageQueue* queue)
{
    transverseR(queue->head, _deleteNode);
}

static void _deleteNode(MessageNode* node)
{
    free(node->msg);
    free(node);
}

int getMessageFromFile(char szBuf[], int iLen)
{
	FILE* file;
	file = fopen("FortuneCookies2.txt", "r");

	// check if the file valid before proceeding
	if (file){

		long fileSize;
		char *fileContent;
		long offset;
		char *offsetString;
		char *next;
		char *message;
		int iMessageLen;

		// get file size
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		// read the file content into memory
		fileContent = (char *) malloc(fileSize);
		fread(fileContent, sizeof(char), fileSize, file);

		// close file as its no longer needed
		fclose(file);

		// get a random position in the file
		offset = rand() % fileSize;
		offsetString = fileContent + offset;

		// find the next occurrence of the delimiter from the random point in the file
		next = strstr(offsetString, MESSAGE_DELIMITER);
		// get the message
		message = strtok(next, MESSAGE_DELIMITER);

		// get the message length
		iMessageLen = strlen(message);

		// copy to the buffer -
		// if message length is less than the max length, use that value.
		// else use the max value
		if (iMessageLen <= iLen){
			strncpy(szBuf, message, iMessageLen-1);
			szBuf[iMessageLen - 1] = '\0';
		}
		else{
			strncpy(szBuf, message, iLen-1);
			szBuf[iLen - 1] = '\0';
		}

		// free file content
		free(fileContent);
	}
	else{
		printf("ooooohhhh noooooo, error opening file :(");
		return FAILURE;
	}

	return SUCCESS;
}

