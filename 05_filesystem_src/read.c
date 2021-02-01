/* read all bytes from file and counts them */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFF_SIZE = 1024;

int readfile(int fd); /* read all file contents and return it size */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    int filesize = readfile(fd);
    if (filesize == -1) {
        perror("readfile");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("sizeof(%s) = %d\n", argv[1], filesize);
    close(fd);
    return EXIT_SUCCESS;
}

int readfile(int fd) {
    int total_size = 0;
    ssize_t rb = 0;
    char buff[BUFF_SIZE];
    while ((rb = read(fd, buff, BUFF_SIZE)) > 0) {
        total_size += rb;
    }
    if (rb == -1) {
        return -1;
    }
    return total_size;
}
