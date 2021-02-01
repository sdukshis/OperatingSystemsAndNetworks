/* Prints variables memory addresses */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define READ_BUFF 1024

const int constant = 42;
int global = 123; /* initialized variable */

char *hello = "hello";

void foo() {} /* just to print function address */

void print_self_maps(); /* prints /proc/self/maps */

int main() {
    int autovar;  /* automatically allocate space */

    void  *smallbuff = malloc(16);
    void  *largebuff = malloc(4096*4096);
    
    print_self_maps();

    printf("&constant:   %p\n", &constant);
    printf("&global:     %p\n", &global);
    printf("hello :      %p\n", hello);
    printf("foo:         %p\n", foo);
    printf("exit:        %p\n", exit);
    printf("gettimeofday:%p\n", gettimeofday);
    printf("autovar:     %p\n", &autovar);
    printf("smallbuff:   %p\n", smallbuff);
    printf("largebuff:   %p\n", largebuff);

    free(smallbuff);
    free(largebuff);
    return 0;
}

void print_self_maps() {
    FILE *fd = fopen("/proc/self/maps", "r");
    if (fd == NULL) {
        perror("fopen");
        return;
    }
    printf("/proc/self/maps content:\n");
    char buff[READ_BUFF];
    size_t rb = 0;
    while ((rb = fread(buff, sizeof(buff[0]), READ_BUFF - 1, fd)) > 0) {
        buff[rb] = '\0';
        printf("%s", buff);
    }
    printf("\n");
    if (!feof(fd)) {
        perror("fread");
        return;
    }
    fclose(fd);
}