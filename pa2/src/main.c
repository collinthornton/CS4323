// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 MAIN
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/book.h"
#include "../include/rating.h"


/*
// @brief Get location of given user
// @param userID (long) userID to be used as input
// @param buff (char*) Buffer to store location
// @return (const char*) Location (same as buff)
*/
const char* getLocation(long userID, char *buff);


const char *HEADER = " \
// ########################################## \r\n \
// \r\n \
//   Author  -   Collin Thornton \r\n \
//   Email   -   collin.thornton@okstate.edu \r\n \
//   Assign  -   Assignment 01 MAIN\r\n \
//   Date    -   9-23-20\r\n \
// \r\n \
// ########################################## ";

// DEFINE IPC NAMES

const char *OUT_QUEUE_NAME  = "/TOMSGPASS";
const char *IN_QUEUE_NAME   = "/TOMAIN";
const char *SHNAME          = "/SHARE";
const char *FLAGNAME        = "/MEMFLAG";

int main(int argc, char** argv) {
    
    printf("\r\n%s\r\n\r\n", HEADER);
    if(argc != 2) {
        printf("Usage: ./main <ISBN>\r\n");
        exit(1);
    }

    char isbn[100];
    strcpy(isbn, argv[1]);


    // DEFINE AND CLEAR MESSAGE QUEUE

    mqd_t in_qd, out_qd;
	if(mq_unlink(IN_QUEUE_NAME)) 	{}
	if(mq_unlink(OUT_QUEUE_NAME)) 	{}

    // DEFINE AND LAUNCH PROCESSES

    const int NUM_PROCESSES = 2;
    pid_t pids[NUM_PROCESSES];

    for(int i=0; i<NUM_PROCESSES; ++i) pids[i] = -1;


    printf("\r\nSEARCHING FOR ISBN: %s\r\n", isbn);

    for(int i=0; i<NUM_PROCESSES; ++i) {
        if((pids[i] = fork()) < 0) {
            perror("Fork failed");
            exit(1);
        }
        if(pids[0] == 0) { 
            
            // EXECUTE SHARED MEMORY CHILD IN SEPERATE PROGRAM
            
            execlp("./shmem", "shmem", NULL);
            exit(0);
        }
        if(pids[1] == 0) {
            
            // EXECUTE MESSAGE PASSING CHILD IN SEPERATE PROGRAM
            
            execlp("./msgpass", "msgpass", NULL);
            exit(0);
        }
    }
    
    sleep(2);

    //  SETUP MESSAGE QUEUE ATTRIBUTES

    struct mq_attr in_attr;
	in_attr.mq_flags 	= MQ_FLAGS;
	in_attr.mq_maxmsg 	= MAX_MESSAGES;	 
	in_attr.mq_msgsize  = MAX_MSG_SIZE;	
	in_attr.mq_curmsgs  = 0;	        

    struct mq_attr out_attr;
	out_attr.mq_flags 	= MQ_FLAGS;
	out_attr.mq_maxmsg 	= MAX_MESSAGES;	 
	out_attr.mq_msgsize = MAX_MSG_SIZE;	
	out_attr.mq_curmsgs = 0;	        


    // DECLARE FILE DESCRIPTORS FOR SHARED MEMORY
    int shm_fd, flag_fd;


    // SETUP POINTERS FOR SHARED MEMORY
    Book *ptr;


    // OPEN SHARED MEMORY FOR FLAG

    flag_fd = shm_open(FLAGNAME, O_CREAT | O_RDWR, 0666);
    if(flag_fd == -1) {
        perror("parent:\tflag_fd");
        printf("errno:\t%d\r\n", errno);
        return(1);
    } 
    ftruncate(flag_fd, sizeof(int));


    // MAP SHARED MEMORY FOR FLAG

    int *flag = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, flag_fd, 0);
    if(flag == MAP_FAILED) {
        perror("Parent:\tflag mmap");
        printf("errno:\t%d\r\n", errno);
        return(1);
    }
   

    // OPEN SHARED MEMORY FOR BOOK

    shm_fd = shm_open(SHNAME, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1) {
        perror("Parent: shm_opem");
        printf("errno: %d\r\n", errno);
        return(1);
    }
    ftruncate(shm_fd, sizeof(Book));


    // MAP SHARED MEMORY FOR BOOK

    ptr = (Book *)mmap(0, sizeof(Book), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        perror("Parent:\t\tmmap");
        printf("errno:\t\t\t%d\r\n", errno);
        return(1);
    }


    // OPEN MESSAGE QUEUE FOR RATING

    if((in_qd = mq_open(IN_QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &in_attr)) == -1) {
        perror("Parent:\t\tmq_open");
        printf("errno:\t\t\t%d\r\n", errno);
        exit(1);
    }

    if((out_qd = mq_open(OUT_QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &out_attr)) == -1) {
        perror("Parent:\t\tmq_open");
        printf("errno:\t\t\t%d\r\n", errno);
        exit(1);
    }

    // DECLARE MEMORY FOR MESSAGE BUFFERS AND INITIALIZE
    char in_buffer[MAX_MSG_SIZE];
    char out_buffer[MAX_MSG_SIZE];
    strcpy(in_buffer, "INIT");
    strcpy(out_buffer, "INIT");

    #ifdef SHMEM_VERBOSE
    printf("MAIN->SHMEM:\tWATING FOR SHMEM TO SET FLAG\r\n");
    #endif


    // WAIT FOR SHMEM CHILD TO SEND READY SIGNAL
    while(*flag != WAITING_FOR_ISBN) sleep(1);


    #ifdef MSGPASS_VERBOSE
    printf("MAIN->MSGPAS:\tWAITING FOR MSGPASS TO SEND READY SIGNAL\r\n");
    #endif


    // WAIT FOR MSGPASS CHILD TO SEND READY SIGNAL
    mq_getattr(in_qd, &in_attr);
    while(strcmp(in_buffer, "READY") != 0) {
        sleep(1);
        mq_getattr(in_qd, &in_attr);

        if(in_attr.mq_curmsgs != 0) {
           mq_receive(in_qd, in_buffer, MAX_MSG_SIZE, NULL);

           #ifdef MSGPASS_VERBOSE
           printf("MAIN->MSGPAS\tRECEIVED: %s\r\n", in_buffer);
           #endif
        }

        #ifdef MSGPASS_VERBOSE
        printf("MAIN->MSGPAS:\tNUM_MESSAGES: %ld\r\n", in_attr.mq_curmsgs);
        #endif
    }

    #ifdef MSGPASS_VERBOSE
    printf("MAIN:\t\tREADY SIGNAL:\t%s\r\n", in_buffer);
    #endif


    // PASS ISBN TO SHMEM CHILD
    Book out_book;
    strcpy(out_book.isbn, isbn);
    memcpy(ptr, &out_book, sizeof(Book));
   
    #ifdef SHMEM_VERBOSE
    printf("MAIN->SHMEM:\tFLAG = ISBN_READY\r\n");
    #endif


    // UPDATE FLAG FOR SHMEM CHILD
    *flag = ISBN_READY;


    // PASS ISBN TO MSGPASS CHILD
    Rating out_rating;
    strcpy(out_rating.isbn, isbn);
    strcpy(out_rating.user, "N/A");
    strcpy(out_rating.rating, "N/A");

    serialize(&out_rating, out_buffer);

    if(mq_send(out_qd, out_buffer, MAX_MSG_SIZE, 0) == -1) {
        perror("Parent:\tUnable to send message");
        printf("errno:\t%d\r\n", errno);
        exit(1);
    }
   
    #ifdef MSGPASS_VERBOSE
    printf("MAIN->MSGPAS:\tSENT %s\t%s\t%s\r\n", out_rating.isbn, out_rating.user, out_rating.rating);
    #endif
    

    // WAIT FOR RESPONSE FROM MSGPASS CHILD
    
    //sleep(1);
    mq_getattr(in_qd, &in_attr);
    while(in_attr.mq_curmsgs == 0) {
        sleep(1);
        mq_getattr(in_qd, &in_attr);
    }


    // DECLARE MEMORY FOR INCOMING RATING AND SETUP A LIST OF RATINGS
    Rating in_rating;
    strcat(in_rating.isbn, "init");
    strcat(in_rating.user, "init");
    strcat(in_rating.rating, "init");

    RatingList rating_list;
    rating_list.num_allocated = 0;
    rating_list.num_ratings = 0;


    // READ ALL INCOMING RATINGS FROM MSGPASS CHILD

    int MAX_RATINGS = 100;
    for(int i=0; i<MAX_RATINGS; ++i) {
        mq_getattr(in_qd ,&in_attr);
        
        #ifdef MSGPASS_VERBOSE
        printf("MAIN:\t\tWAITING FOR MSG %d\r\n", i);
        #endif

        while(in_attr.mq_curmsgs == 0) {
            sleep(1);
            mq_getattr(in_qd, &in_attr);
        }
        mq_receive(in_qd, in_buffer, MSG_BUFFER_SIZE, NULL);

        #ifdef MSGPASS_VERBOSE
        printf("MAIN:\t\tCALLING DESERIALIZE\r\n");
        #endif

        deserialize(in_buffer, &in_rating);


        // EXIT LOOP UPON RECEIPT OF LAST RATING

        if(strcmp(in_rating.isbn, "NULL") == 0 && strcmp(in_rating.user, "NULL") == 0 && strcmp(in_rating.rating, "NULL") == 0) break;

        // OTHERWISE ADD TO LIST
        
        addRatingToList(&in_rating, &rating_list);
        
        #ifdef MSGPASS_VERBOSE
        printf("MAIN->MSGPAS:\tRECEIVED %s\t%s\t%s\r\n", in_rating.isbn, in_rating.user, in_rating.rating);
        #endif
    }

    #ifdef SHMEM_VERBOSE
    printf("MAIN->SHMEM:\tWAITING FOR BOOK\r\n");
    #endif


    // WAIT FOR SHMEM TO UPDATE & SETUP LOCAL BOOK

    Book in_book;

    while(*flag == ISBN_READY || *flag == SEARCHING_FOR_BOOK) { 
        sleep(1);
        #ifdef SHMEM_VERBOSE
        switch(*flag) {
            case BOOK_FOUND:
                printf("MAIN->SHMEM:\tFLAG = BOOK_FOUND\r\n");
                memcpy(&in_book, ptr, sizeof(Book));
                break;
            case BOOK_NOT_FOUND:
                printf("MAIN->SHMEM:\tFLAG = BOOK_NOT_FOUND\r\n");
                break;
            case NOT_READY:
                printf("MAIN->SHMEM:\tFLAG = NOT_READY\r\n");
                break;
            case ISBN_READY:
                printf("MAIN->SHMEM:\tFLAG = ISBN_READY\r\n");
                break;
            case SEARCHING_FOR_BOOK:
                printf("MAIN->SHMEM:\tFLAG = SEARCHING_FOR_BOOK\r\n");
                break;
            case WAITING_FOR_ISBN:
                printf("MAIN->SHMEM:\tFLAG = WAITING_FOR_ISBN\r\n");
                break;
            default:
                printf("MAIN->SHMEM:\tFLAG UNRECOGNIZED: %d\r\n", *flag);
                break; 
        }
        #endif
    }


    // OUTPUT DATA

    printf("\r\n\r\n|----------------------------------------------------------------------------------------------------|\r\n|\r\n");
    if(*flag == BOOK_FOUND) {
        printf("| BOOK FOUND ------ ISBN: %10s  TITLE: %20s  -- AUTHOR: %15s  -- PUBLISHER: %10s  -- YEAR: %d\r\n", ptr->isbn, ptr->title, ptr->author, ptr->publisher, ptr->year);
    } else if (*flag == BOOK_NOT_FOUND) {
        printf("| BOOK NOT FOUND\r\n");
    } else {
        printf("| MAIN->SHMEM:\tFLAG = UNEXPECTED STATE\r\n");
    }
   
    printf("|\r\n");
    if(rating_list.num_ratings > 0) {
        for(int i=0; i<rating_list.num_ratings; ++i) {
            char locationBuff[50];
            printf("|  RATING FOUND --- ISBN: %10s  USER: %10s  RATING:%5s  LOCATION:%15s\r\n", rating_list.rating_list[i].isbn, rating_list.rating_list[i].user, rating_list.rating_list[i].rating, getLocation(atoi(rating_list.rating_list[i].user), locationBuff));
        }
    } else {
        printf("|  NO RATINGS FOUND");
    }

    printf("|\r\n|----------------------------------------------------------------------------------------------------|\r\n\r\n\r\n");


    // WAIT FOR CHILD TERMINATION

    for(int i=0; i<NUM_PROCESSES; ++i) {
        pid_t died = wait(NULL);
        if(died == pids[0]) {
            #ifdef SHMEM_VERBOSE
            printf("TERMINATED\tSHMEM\r\n");
            #endif
        }
        else if(died == pids[1]) {
            #ifdef MSGPASS_VERBOSE
            printf("TERMINATED\tMSGPAS\r\n");
            #endif
        }
    }


    // CLEANUP MEMORY

    destroyRatingList(&rating_list);

    munmap(flag, sizeof(int));
    close(flag_fd);
    shm_unlink(FLAGNAME);

    munmap(ptr, sizeof(Book));
    close(shm_fd);
    shm_unlink(SHNAME);

	if(mq_close(in_qd) == -1) 		{}
	if(mq_unlink(IN_QUEUE_NAME)) 	{}
	if(mq_close(out_qd) == -1) 		{}
	if(mq_unlink(OUT_QUEUE_NAME)) 	{}


    // EXIT

    return 0;
}



/*
// @brief Get location of given user
// @param userID (long) userID to be used as input
// @param buff (char*) Buffer to store location
// @return (const char*) Location (same as buff)
*/
const char* getLocation(long userID, char *locationBuff) {
    FILE *file = fopen("../data/Users.txt", "r");

    if(file == NULL) {
        perror("users: Input file not found");
        exit(1);
    }

    char buff[1024];

    fgets(buff, 1024, file);

    #ifdef SHMEM_VERBOSE
    printf("\r\n\r\n");
    #endif

    long j=0;
    char fields[3][256];


    // SCAN THROUGH FILE FOR USERID

    while(j != userID && fgets(buff, 1024, file) != NULL) {
        char *tmp = strtok(buff, ";");

        int i = 0;
        while(tmp != NULL && i < 3) {
            strcpy(fields[i++], tmp);

            tmp = strtok(NULL, ";");
        }

        j = atoi(fields[0]);
    }


    // IF FOUND, OUTPUT LOCATION. ELSE OUTPUT "N/A"

    if(j != userID) strcpy(locationBuff, "N/A");
    else strcpy(locationBuff, fields[1]);

    fclose(file);

    return(locationBuff);
}