// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 BOOK DEFINITIONS
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/book.h"


void createBookList(const char *input_file, BookList *book_list) {
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
        if(book_list->num_books%20000 == 0) printf(".");
        #endif
        
        if(book_list->num_allocated == 0) {
            book_list->book_list = (Book*)malloc(2*sizeof(Book));
            book_list->num_allocated = 2;
        }    
        else if(book_list->num_books >= book_list->num_allocated/2) {
            book_list->book_list = (Book*)realloc(book_list->book_list, 2*book_list->num_allocated*sizeof(Book));
            book_list->num_allocated *= 2;
        }

        strcpy(book_list->book_list[book_list->num_books].isbn, fields[0]);
        strcpy(book_list->book_list[book_list->num_books].title, fields[1]);
        strcpy(book_list->book_list[book_list->num_books].author, fields[2]);
        book_list->book_list[book_list->num_books].year = atoi(fields[3]);
        strcpy(book_list->book_list[book_list->num_books].publisher, fields[4]);
        ++book_list->num_books;
    }

    fclose(file);
}

void destroyBookList(BookList *book_list) {
    free(book_list->book_list);
}

Book* srchDatabase(const char *isbn, BookList* book_list) {
    for(int i=0; i<book_list->num_books; ++i) {
        if(strcmp(isbn, book_list->book_list[i].isbn) == 0) return &book_list->book_list[i];
    }
    return NULL;
}