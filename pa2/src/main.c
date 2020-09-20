// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 01 MAIN
//   Date    -   9-23-20
//
// ########################################## 

#include <stdio.h>		// for printf
#include <stdlib.h>		// for exit(1)
#include <string.h>		// for string operation: strlen
#include <errno.h>		// for system error numbers
#include <time.h>		// for clock()

#include <unistd.h>		// for fork
#include <sys/wait.h>	// for wait

#include <sys/ipc.h> 	// Used by IPC mechanisms: messages, shared memory and semaphores
#include <sys/msg.h>	// for message queue structures
#include <mqueue.h>		// for mq_close, mq_unlink, ...

#include <sys/shm.h>    // used to open, close shared memory
#include <sys/stat.h>   // Used to configure size of memory
#include <sys/mman.h>   // Used to create memory map
#include <fcntl.h>		// for file control options

#define NUM_ELEMS 30

typedef struct {
    int test1, test2;
} Test;


int main(int argc, char** argv) {
    printf("Main process: PID=%d\n", getpid());

    const int NUM_PROCESSES = 1;

    const int SHM_SIZE = sizeof(Test[NUM_ELEMS]);
    const char *name = "SHARE";

    int shm_fd;
    Test *ptr, msg;



    pid_t pids[NUM_PROCESSES];

    for(int i=0; i<NUM_PROCESSES; ++i) {
        if((pids[i] = fork()) < 0) {
            perror("Fork failed");
            exit(1);
        }
        if(pids[i] == 0) { 
            execlp("./shmem", "shmem", NULL);
           // printf("PID%d says hi\n", getpid());
            exit(0);
        }
    }


    // PARENT PROCESS
    printf("Waiting for threads to exit\n");
    for(int i=0; i<NUM_PROCESSES; ++i) {
        pid_t died = wait(NULL);
        printf("TERMINATED PID%d\n", died);
    }

    shm_fd = shm_open(name, O_RDWR, 0666);
    if(shm_fd == -1) {
        printf("Error opening file descriptor\r\n");
        printf("errno: %d\r\n", errno);
        return(1);
    }

    ptr = (Test *)mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        printf("Mapping failed\r\n");
        printf("errno: %d\r\n", errno);
        return(1);
    }
    for(int i=0; i<NUM_ELEMS; ++i) printf("RECEIVED: %d, %d\r\n", ptr[i].test1, ptr[i].test2);
    
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(name);
    return 0;
}