/* Use brk syscall to allocate memory */

#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

int main() {
    char *p = sbrk(0);
    printf("p = %p\n", p);
    if(brk(p + 0x1000)) {
        perror("brk");
        return EXIT_FAILURE;
    }
    p = sbrk(0);
    printf("p = %p\n", p);
    if(brk(p + 100)) {
        perror("brk");
        return EXIT_FAILURE;
    }
    printf("*(p + 100): %x\n", *(p + 100));
    printf("*(p + 200): %x\n", *(p + 200));
    printf("*(p + 0x1000): %x\n", *(p + 0x1000)); /* segmentation fault */
    return 0;
}