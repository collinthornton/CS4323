#ifndef SHMEM_H
#define SHMEM_H

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

const char *SHNAME = "/SHARE";
const char *FLAGNAME = "/MEMFLAG";

typedef struct {
    char isbn[32];
    char title[256];
    char author[128];
    char publisher[128];
    unsigned int year;
} Book;

typedef struct {
    Book *book_list;
    int num_allocated, num_books;
} BookList;

void createBookList(const char *input_file, BookList *book_list);
void destroyBookList(BookList *book_list);

Book* srchDatabase(const char *isbn, BookList *book_list);


#endif // SHMEM_H