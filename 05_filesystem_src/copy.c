/* copy file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFF_SIZE = 1024;

int copy(int srcd, int dstd); /* copy contents from srcd to dstd */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int srcd = open(argv[1], O_RDONLY);
    if (srcd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    int dstd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (dstd == -1) {
        perror("open");
        close(srcd);
        return EXIT_FAILURE;
    }
    if (-1 == copy(srcd, dstd)) {
        perror("copy");
        close(srcd);
        close(dstd);
        return EXIT_FAILURE;
    }
    close(srcd);
    close(dstd);
    return EXIT_SUCCESS;
}

int copy(int srcd, int dstd) {
    char buff[BUFF_SIZE];
    int rb = 0;
    int total_bytes = 0;
    while ((rb = read(srcd, buff, BUFF_SIZE)) > 0) {
        total_bytes += rb;
        int remb = rb;
        char *pbuff = buff;
        do {
            int wb = write(dstd, pbuff, remb);
            if (wb == -1) {
                return -1;
            }
            pbuff += wb;
            remb -= wb;
        } while (remb);
    }
    if (rb == -1) {
        return -1;
    }
    return total_bytes;
}
