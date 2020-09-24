// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 RATING DECLARATIONS
//   Date    -   9-23-20
//
// ########################################## 

#ifndef RATING_H
#define RATING_H


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

//#define MSGPASS_VERBOSE


typedef struct {
    char isbn[32];
    char user[32];
    char rating[5];
} Rating;
typedef struct{
    Rating* rating_list;
    int num_ratings, num_allocated;
} RatingList;


void addRatingToList(Rating *rating, RatingList *rating_list);
void createRatingList(const char *input_file, RatingList *rating_list);
void destroyRatingList(RatingList *book_list);

RatingList* srchRatingList(const char *isbn, RatingList *rating_list, RatingList *out_ratings);

const char* serialize(Rating *rating, char* msg);
Rating* deserialize(const char *msg, Rating *rating);


#endif // RATING_H