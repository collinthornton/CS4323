#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/shm.h>    // used to open, close shared memory
#include <sys/stat.h>   // Used to configure size of memory
#include <sys/mman.h>   // Used to create memory map
#include <fcntl.h>		// for file control options

#define NUM_ELEMS 30

typedef struct {
    int test1, test2;
} Test;

int main() {
    printf("LAUNCHED: PID%d\r\n", getpid());
    const int SHM_SIZE = sizeof(Test[NUM_ELEMS]);
    const char *name = "SHARE";

    int shm_fd;
    Test *ptr;

    Test msg[NUM_ELEMS];
    for(int i=0; i<NUM_ELEMS; ++i) {
        msg[i].test1 = i;
        msg[i].test2 = -i;
    }

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);

    ptr = (Test*)mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    memcpy(ptr, msg, SHM_SIZE);

    for(int i=0; i<NUM_ELEMS; ++i) printf("WROTE\t%d, %d\r\n", ptr[i].test1, ptr[i].test2);
  
    return(0);
}