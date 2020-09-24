// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 MSGPASS
//   Date    -   9-23-20
//
// ########################################## 


#include "../include/rating.h"

const char *OUT_QUEUE_NAME 	= "/TOMAIN";
const char *IN_QUEUE_NAME 	= "/TOMSGPASS";

int main() 
{ 

	#ifdef MSGPASS_VERBOSE
	printf("LAUNCH:\t\tMSGPAS %d\r\n", getpid());
	#endif

	mqd_t in_qd, out_qd;   // queue descriptors
	if(mq_close(out_qd) == -1) 		{}
	if(mq_unlink(OUT_QUEUE_NAME)) 	{}	
	if(mq_close(in_qd) == -1) 		{}
	if(mq_unlink(IN_QUEUE_NAME)) 	{}

	struct mq_attr in_attr;
	in_attr.mq_flags 	= MQ_FLAGS;
	in_attr.mq_maxmsg 	= MAX_MESSAGES;	// The maximum number of messages that can be stored on the queue. 
	in_attr.mq_msgsize  = MAX_MSG_SIZE;	// The maximum size of each message on the given message queue. 
	in_attr.mq_curmsgs  = 0;			// This field represents the number of messages currently on the given queue.

	struct mq_attr out_attr;
	out_attr.mq_flags	= MQ_FLAGS;
	out_attr.mq_maxmsg	= MAX_MESSAGES;
	out_attr.mq_msgsize	= MAX_MSG_SIZE;
	out_attr.mq_curmsgs = 0;


	if ((in_qd = mq_open (IN_QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &in_attr)) == -1) {
		perror("msgpass:\tmq_open");
		printf("errno:\t%d\r\n", errno);
		exit(1);
	}
	
	if ((out_qd = mq_open (OUT_QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &out_attr)) == -1) {
		perror("msgpass:\tmq_open");
		printf("errno:\t%d\r\n", errno);
		exit(1);
	}
	char in_buffer[MSG_BUFFER_SIZE];
	char out_buffer[MSG_BUFFER_SIZE];

	#ifdef MSGPASS_VERBOSE
	printf("MSGPAS->MAIN:\tWAITING FOR ISBN\r\n");
	#endif

	strcpy(out_buffer, "READY");
	if(mq_send(out_qd, out_buffer, MAX_MSG_SIZE, 0) == -1) {
		perror("msgpass:\tsend READY flag");
		printf("errno:\t%d\r\n", errno);
		exit(1);
	}

	while(in_attr.mq_curmsgs == 0) { 
		sleep(1); 
		mq_getattr(in_qd, &in_attr); 

		#ifdef MSGPASS_VERBOSE
		printf("MSGPAS->MAIN\tNUM_MESSAGES: %ld\r\n", in_attr.mq_curmsgs);
		#endif
	}

	mq_receive(in_qd, in_buffer, MSG_BUFFER_SIZE, NULL);

	Rating in_rating;

	#ifdef MSGPASS_VERBOSE
	printf("MSGPAS\t\tCALLING DESERIALIZE\r\n");
	#endif

	deserialize(in_buffer, &in_rating);


	#ifdef MSGPASS_VERBOSE
	printf("MSGPAS->MAIN:\tRECEIVED: %s\t%s\t%s\r\n", in_rating.isbn, in_rating.user, in_rating.rating);
	#endif


	RatingList rating_list;
	rating_list.num_allocated = 0;
	rating_list.num_ratings = 0;

	createRatingList("../data/Ratings.txt", &rating_list);

	RatingList out_ratings;
	out_ratings.num_allocated = 0;
	out_ratings.num_ratings = 0;
	
	

	if(	srchRatingList(in_rating.isbn, &rating_list, &out_ratings) == NULL) {
		#ifdef MSGPASS_VERBOSE
		printf("MSGPAS->MAIN:\tRATING NOT FOUND\r\n");
		#endif

		mq_send(out_qd, "RATING_NOT_FOUND", MAX_MSG_SIZE, 0);
	} else {
		#ifdef MSGPASS_VERBOSE
		printf("MSGPAS->MAIN:\rRATING FOUND\r\n");
		#endif
		for(int i=0; i<out_ratings.num_ratings; ++i) {
			#ifdef MSGPASS_VERBOSE
			printf("MSGPAS->MAIN:\tRATING\t%s\t%s\t%s\r\n", out_ratings.rating_list[i].isbn, out_ratings.rating_list[i].user, out_ratings.rating_list[i].rating);
			#endif

			serialize(&out_ratings.rating_list[i], out_buffer);
			if(mq_send(out_qd, out_buffer, MAX_MSG_SIZE, 0) == -1) {
				perror("msgpass:\tsned rating");
				printf("errno:\t%d\r\n", errno);
				exit(1);
			}
		}
	}

	destroyRatingList(&out_ratings);
	destroyRatingList(&rating_list);

	#ifdef MSGPASS_VERBOSE
	printf("MSGPAS->MAIN:\tEXITING\r\n");		
	#endif

	return 0; 
} 