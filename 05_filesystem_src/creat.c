/* Create empty file */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = creat(argv[1], 0666); // mode will be affected by umask
    if (fd == -1) {
        perror("creat");
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}
