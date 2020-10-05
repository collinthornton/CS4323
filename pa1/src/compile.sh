#!/bin/bash

gcc rating.c book.c main.c -lrt -o main -g
gcc book.c shmem.c -lrt -o shmem -g
gcc rating.c msgpass.c -lrt -o msgpass -g