// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 BOOK DEFINITIONS
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/book.h"


/*
// @brief Generate a list of books allocated on heap
// @param input_file (const char*) filenames to be used as input
// @param book_list (BookList) Struct to store list
*/
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

    // SCAN FILE AND INPUT ALL FIELDS

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
        
        
        // ALLOCATE MEMORY FOR BOOK
        
        if(book_list->num_allocated == 0) {
            book_list->book_list = (Book*)malloc(2*sizeof(Book));
            book_list->num_allocated = 2;
        }    
        else if(book_list->num_books >= book_list->num_allocated/2) {
            book_list->book_list = (Book*)realloc(book_list->book_list, 2*book_list->num_allocated*sizeof(Book));
            book_list->num_allocated *= 2;
        }


        // COPY DATA

        strcpy(book_list->book_list[book_list->num_books].isbn, fields[0]);
        strcpy(book_list->book_list[book_list->num_books].title, fields[1]);
        strcpy(book_list->book_list[book_list->num_books].author, fields[2]);
        book_list->book_list[book_list->num_books].year = atoi(fields[3]);
        strcpy(book_list->book_list[book_list->num_books].publisher, fields[4]);
        ++book_list->num_books;
    }

    fclose(file);
}

/*
// @brief Free data from heap
// @param book_list (BookList*) List to be freed
*/
void destroyBookList(BookList *book_list) {
    free(book_list->book_list);
}

/*
// @brief Search a book list for isbn
// @param isbn (const char*) isbn to be used as key
// @param book_list (BookList*) List to search
// @return (Book*) Book with given isbn
*/
Book* srchDatabase(const char *isbn, BookList* book_list) {
    for(int i=0; i<book_list->num_books; ++i) {
        if(strcmp(isbn, book_list->book_list[i].isbn) == 0) return &book_list->book_list[i];
    }
    return NULL;
}