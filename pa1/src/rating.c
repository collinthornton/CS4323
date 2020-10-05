// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 RATING DEFINITIONS
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/rating.h"


/*
// @brief Generate a list of ratings allocated on heap
// @param input_file (const char*) filenames to be used as input
// @param rating_list (RatingList) Struct to store list
*/
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


    // SCAN FILE AND INPUT ALL FIELDS

    while(fgets(buff, 1024, file) != NULL) {
        char fields[8][256];


        char *tmp = strtok(buff, "\";\"");

        int i = 0;


        // WE ONLY CARE ABOUT THE FIRST FIVE FIELDS

        while(tmp != NULL && i < 5) {
            strcpy(fields[i++], tmp);

            tmp = strtok(NULL, "\";\"");
        }

        #ifdef SHMEM_VERBOSE
        if(rating_list->num_ratings%20000 == 0) printf(".");
        #endif


        // ADD RATING TO LIST

		Rating rating;
        strcpy(rating.user, fields[0]);
        strcpy(rating.isbn, fields[1]);
        strcpy(rating.rating, fields[2]);

		addRatingToList(&rating, rating_list);
    }

    #ifdef SHMEM_VERBOSE
    printf("\r\n\r\n");
    #endif

    fclose(file);
}

/*
// @brief Add a heap-allocated rating to a list
// @param rating (Rating*) Rating to be added
// @param rating_list (RatingList*) RatingList to store rating
*/
void addRatingToList(Rating *rating, RatingList *rating_list) {

    // ALLOCATE MEMORY

    if(rating_list->num_allocated == 0) {
        rating_list->rating_list = (Rating*)malloc(2*sizeof(Rating));
        rating_list->num_allocated = 2;
    }    
    else if(rating_list->num_ratings >= rating_list->num_allocated/2) {
        rating_list->rating_list = (Rating*)realloc(rating_list->rating_list, 2*rating_list->num_allocated*sizeof(Rating));
        rating_list->num_allocated *= 2;
    }


    // COPY DATA
    strcpy(rating_list->rating_list[rating_list->num_ratings].user, rating->user);
    strcpy(rating_list->rating_list[rating_list->num_ratings].isbn, rating->isbn);
    strcpy(rating_list->rating_list[rating_list->num_ratings].rating, rating->rating);
    ++rating_list->num_ratings;
}


/*
// @brief Free data from heap
// @param rating_list (RatingList*) List to be freed
*/
void destroyRatingList(RatingList *rating_list) {
    free(rating_list->rating_list);
}

/*
// @brief Search a rating list
// @param isbn (const char*) isbn to be used as key
// @param rating_list (RatingList*) List to search
// @param out_ratings (RatingList*) List of ratings with isbn
// @return (RatingList*) Full list of ratings for given isbn
*/
RatingList* srchRatingList(const char *isbn, RatingList* rating_list, RatingList* out_ratings) {
    for(int i=0; i<rating_list->num_ratings; ++i) {
        if(strcmp(isbn, rating_list->rating_list[i].isbn) == 0) addRatingToList(&rating_list->rating_list[i], out_ratings);
    }
	if(out_ratings->num_ratings == 0) return NULL;

    return out_ratings;
}


/*
// @brief Convert rating to string
// @param rating (Rating*) Rating to be converted
// @param msg (char*) Buffer to store string
// @return (const char*) String version of rating (same as buff)
*/
const char* serialize(Rating *rating, char* msg) {
    strcpy(msg, "RATING;Isbn:");
    strcat(msg, rating->isbn);
    strcat(msg, ";User:");
    strcat(msg, rating->user);
    strcat(msg, ";Rating:");
    strcat(msg, rating->rating);
    return msg;
}

/*
// @brief Convert string to rating
// @param msg (const char*) string to be converted
// @param rating (Rating*) Rating to store msg
// @return (Rating*) rating struct
*/
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