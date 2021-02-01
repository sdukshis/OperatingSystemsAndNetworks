/* create file and write zeros to it */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFF_SIZE = 1024;

int filln(int fd, int size, char val); /* write val into fd size times */

int main(int argc, char *argv[]) {
    
}