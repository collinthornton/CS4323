// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 SHMEM
//   Date    -   9-23-20
//
// ########################################## 

#include "../include/book.h"


const char *SHNAME      = "/SHARE";
const char *FLAGNAME    = "/MEMFLAG";


int main(int argc, char** argv) {
    
    #ifdef SHMEM_VERBOSE
    printf("LAUNCH:\t\tSHMEM PID%d\r\n", getpid());
    #endif

    const int SHM_SIZE = sizeof(Book[NUM_ELEMS]);

    // Setup shared memory for book exchange
    int shm_fd = shm_open(SHNAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);

    Book *ptr = (Book*)mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    // Setup shared memory for status flag
    int flag_fd = shm_open(FLAGNAME, O_CREAT | O_RDWR, 0666);
    ftruncate(flag_fd, sizeof(int));

    int *flag = (int*)mmap(0, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, flag_fd, 0);
    
    
    // Set flag to waiting and wait
    *flag = WAITING_FOR_ISBN;

    #ifdef SHMEM_VERBOSE
    printf("SHMEM->MAIN:\tWAITING FOR ISBN\r\n");
    #endif
    
    while(*flag != ISBN_READY) sleep(1);
    
    #ifdef SHMEM_VERBOSE
    printf("SHMEM->MAIN:\tFLAG = SEARCHING_FOR_BOOK\tISBN:%s\r\n", ptr->isbn);
    #endif

    *flag = SEARCHING_FOR_BOOK;



    BookList book_list;
    book_list.num_allocated = 0;
    book_list.num_books = 0;

    createBookList("../data/Books.txt", &book_list);

    #ifdef SHMEM_VERBOSE
    printf("\r\n\r\n");
    #endif
    
    Book* book = srchDatabase(ptr->isbn, &book_list);

    if(book == NULL) {
        #ifdef SHMEM_VERBOSE
        printf("SHMEM->MAIN:\tFLAG = BOOK_NOT_FOUND\r\n");
        #endif

        *flag = BOOK_NOT_FOUND;
    } else {
        #ifdef SHMEM_VERBOSE
        printf("SHMEM->MAIN:\tFLAG = BOOK_FOUND\t%s\t%s\r\n", book->isbn, book->title);
        #endif
        
        memcpy(ptr, book, sizeof(Book));
        *flag = BOOK_FOUND;
    }
      
    destroyBookList(&book_list);

    #ifdef SHMEM_VERBOSE
    printf("SHMEM->MAIN:\tEXITING\r\n");
    #endif 

    return(0);
}

