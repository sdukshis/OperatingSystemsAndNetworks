/* Allocate memory with mmap */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main() {
    char *p = mmap(NULL, 2000, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }
    *(p + 2000) = 0;
    *(p + 3000) = 0;
    *(p + 4096) = 0; /* segmentation fault */
    if (-1 == munmap(p, 2000)) {
        perror("munmap");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}