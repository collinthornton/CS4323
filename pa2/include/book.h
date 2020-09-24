// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 BOOK INCLUDE
//   Date    -   9-23-20
//
// ########################################## 

#ifndef BOOK_H
#define BOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/shm.h>    // used to open, close shared memory
#include <sys/stat.h>   // Used to configure size of memory
#include <sys/mman.h>   // Used to create memory map
#include <fcntl.h>		// for file control options

#define NUM_ELEMS 1

#define ISBN_READY 0
#define NOT_READY 1
#define SEARCHING_FOR_BOOK 2
#define BOOK_NOT_FOUND 3
#define BOOK_FOUND 4
#define WAITING_FOR_ISBN 5

//#define SHMEM_VERBOSE


// STRUCT TO CONTAIN BOOK INFORMATION
typedef struct {
    char isbn[32];
    char title[256];
    char author[128];
    char publisher[128];
    unsigned int year;
} Book;


// STRUCT TO CONTAIN LIST OF BOOKS
typedef struct {
    Book *book_list;
    int num_allocated, num_books;
} BookList;

/*
// @brief Generate a list of books allocated on heap
// @param input_file (const char*) filenames to be used as input
// @param book_list (BookList) Struct to store list
*/
void createBookList(const char *input_file, BookList *book_list);

/*
// @brief Free data from heap
// @param book_list (BookList*) List to be freed
*/
void destroyBookList(BookList *book_list);


/*
// @brief Search a book list for isbn
// @param isbn (const char*) isbn to be used as key
// @param book_list (BookList*) List to search
// @return (Book*) Book with given isbn
*/
Book* srchDatabase(const char *isbn, BookList *book_list);


#endif // BOOK_H