# Programming Assignment 01

## Compilation Instructions

>*__Note__: c files should be in src directory*
>*__Note__: h files should be in include directory*


File structure:
/pa2
---/src
---/include
---/data

Compilation commands (in src directory):

`gcc rating.c book.c main.c -lrt -o main -g`
`gcc book.c shmem.c -lrt -o shmem -g`
`gcc rating.c msgpass.c -lrt -o msgpass -g`

Or, simply run (in src directory):
`./compile.sh`


Execution instructions (in src directory):
- __Data should be in data/ directory__
`./main <ISBN>`
