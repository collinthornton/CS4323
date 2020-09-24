// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 RATING DEFINITIONS
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/rating.h"


void createRatingList(const char *input_file, RatingList *rating_list) {
    FILE *file = fopen(input_file, "r");

    if(file == NULL) {
        perror("shmem: Input file not found");
        exit(1);
    }

    char buff[1024];

    fgets(buff, 1024, file);

    #ifdef SHMEM_VERBOSE
    printf("\r\n\r\n");
    #endif

    while(fgets(buff, 1024, file) != NULL) {
        char fields[8][256];


        char *tmp = strtok(buff, "\";\"");

        int i = 0;
        while(tmp != NULL && i < 5) {
            strcpy(fields[i++], tmp);

            tmp = strtok(NULL, "\";\"");
        }

        #ifdef SHMEM_VERBOSE
        if(rating_list->num_ratings%20000 == 0) printf(".");
        #endif
        
        if(rating_list->num_allocated == 0) {
            rating_list->rating_list = (Rating*)malloc(2*sizeof(Rating));
            rating_list->num_allocated = 2;
        }    
        else if(rating_list->num_ratings >= rating_list->num_allocated/2) {
            rating_list->rating_list = (Rating*)realloc(rating_list->rating_list, 2*rating_list->num_allocated*sizeof(Rating));
            rating_list->num_allocated *= 2;
        }

		Rating rating;
        strcpy(rating.user, fields[0]);
        strcpy(rating.isbn, fields[1]);
        strcpy(rating.rating, fields[2]);

		addRatingToList(&rating, rating_list);

        ++rating_list->num_ratings;
    }

    printf("\r\n\r\n");
    fclose(file);
}

void addRatingToList(Rating *rating, RatingList *rating_list) {
	    if(rating_list->num_allocated == 0) {
            rating_list->rating_list = (Rating*)malloc(2*sizeof(Rating));
            rating_list->num_allocated = 2;
        }    
        else if(rating_list->num_ratings >= rating_list->num_allocated/2) {
            rating_list->rating_list = (Rating*)realloc(rating_list->rating_list, 2*rating_list->num_allocated*sizeof(Rating));
            rating_list->num_allocated *= 2;
        }

        strcpy(rating_list->rating_list[rating_list->num_ratings].user, rating->user);
        strcpy(rating_list->rating_list[rating_list->num_ratings].isbn, rating->isbn);
        strcpy(rating_list->rating_list[rating_list->num_ratings].rating, rating->rating);
        ++rating_list->num_ratings;
}

void destroyRatingList(RatingList *rating_list) {
    free(rating_list->rating_list);
}

RatingList* srchRatingList(const char *isbn, RatingList* rating_list, RatingList* out_ratings) {
    for(int i=0; i<rating_list->num_ratings; ++i) {
        if(strcmp(isbn, rating_list->rating_list[i].isbn) == 0) addRatingToList(&rating_list->rating_list[i], out_ratings);
    }
	if(out_ratings->num_ratings == 0) return NULL;

    return out_ratings;
}


const char* serialize(Rating *rating, char* msg) {
    strcpy(msg, "RATING;Isbn:");
    strcat(msg, rating->isbn);
    strcat(msg, ";User:");
    strcat(msg, rating->user);
    strcat(msg, ";Rating:");
    strcat(msg, rating->rating);
    return msg;
}

Rating* deserialize(const char *msg, Rating *rating) {
	char buff[MAX_MSG_SIZE];
	strcpy(buff, msg);

    char *tmp = strtok(buff, ";");
    if(strcmp(tmp, "RATING") != 0) {
        printf("rating deserialize: RATING unrecognized\t%s\t%s\r\n", msg ,tmp);
    }

    tmp = strtok(NULL, ":");
    if(strcmp(tmp, "Isbn") != 0) {
        printf("rating deserialize: Isbn unrecognized\t\t%s\t%s\r\n", msg ,tmp);
    }    

    tmp = strtok(NULL, ";");
    if(tmp == NULL) {
        printf("rating deserialize: isbn unrecognized\t\t%s\t%s\r\n", msg ,tmp);
    }
    strcpy(rating->isbn, tmp);

    tmp = strtok(NULL, ":");
    if(strcmp(tmp, "User") != 0) {
        printf("rating deserialize: User unrecognized\t%s\t%s\r\n", msg ,tmp);
    }    

    tmp = strtok(NULL, ";");
    if(tmp == NULL) {
        printf("rating deserialize: user unrecognized\t%s\t%s\r\n", msg ,tmp);
    }
    strcpy(rating->user, tmp);

    tmp = strtok(NULL, ":");
    if(strcmp(tmp, "Rating") != 0) {
        printf("rating deserialize: Rating unrecognized\t%s\t%s\r\n", msg ,tmp);
    }    

    tmp = strtok(NULL, ";");
    if(tmp == NULL) {
        printf("rating deserialize: rating unrecognized\t%s\t%s\r\n", msg ,tmp);
    }
    strcpy(rating->rating, tmp);


    return rating;
}