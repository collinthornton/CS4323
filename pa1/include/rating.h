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

// #define MSGPASS_VERBOSE

// STRUCT TO CONTAIN RATING INFORMATION
typedef struct {
    char isbn[32];
    char user[32];
    char rating[5];
} Rating;

// STRUCT TO CONTAIN LIST OF RATINGS
typedef struct{
    Rating* rating_list;
    int num_ratings, num_allocated;
} RatingList;

/*
// @brief Generate a list of ratings allocated on heap
// @param input_file (const char*) filenames to be used as input
// @param rating_list (RatingList) Struct to store list
*/
void createRatingList(const char *input_file, RatingList *rating_list);

/*
// @brief Add a heap-allocated rating to a list
// @param rating (Rating*) Rating to be added
// @param rating_list (RatingList*) RatingList to store rating
*/
void addRatingToList(Rating *rating, RatingList *rating_list);

/*
// @brief Free data from heap
// @param rating_list (RatingList*) List to be freed
*/
void destroyRatingList(RatingList *book_list);


/*
// @brief Search a rating list
// @param isbn (const char*) isbn to be used as key
// @param rating_list (RatingList*) List to search
// @param out_ratings (RatingList*) List of ratings with isbn
// @return (RatingList*) Full list of ratings for given isbn
*/
RatingList* srchRatingList(const char *isbn, RatingList *rating_list, RatingList *out_ratings);

/*
// @brief Convert rating to string
// @param rating (Rating*) Rating to be converted
// @param msg (char*) Buffer to store string
// @return (const char*) String version of rating (same as buff)
*/
const char* serialize(Rating *rating, char* msg);

/*
// @brief Convert string to rating
// @param msg (const char*) string to be converted
// @param rating (Rating*) Rating to store msg
// @return (Rating*) rating struct
*/
Rating* deserialize(const char *msg, Rating *rating);


#endif // RATING_H