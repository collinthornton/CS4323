#ifndef MSGPASS_H
#define MSGPASS_H


#include <stdio.h>		// for printf
#include <time.h>		// for clock()
#include <unistd.h>		// for fork
#include <stdlib.h>		// for exit(1)
#include <sys/ipc.h> 	// Used by IPC maechanisms: messages, shared memory and semaphores
#include <sys/msg.h>	// for message queue structures
#include <string.h>		// for string operation: strlen
#include <sys/wait.h>	// for wait
#include <fcntl.h>		// for file control options
#include <errno.h>		// for system error numbers
#include <mqueue.h>		// for mq_close, mq_unlink, ...


#define PERMISSIONS 0660

#define MAX_MESSAGES    10
#define MAX_MSG_SIZE    256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#define MQ_FLAGS        0
#define MQ_CURMSGS      0

const char *QUEUE_NAME = "/QUEUE";


#endif //MSGPASS_H