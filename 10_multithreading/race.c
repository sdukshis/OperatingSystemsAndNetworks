/* demostrate race condition by incrementing integer */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

int gNumber = 0;

void *thread_start(void *number) {
    int *numref = (int*)number;

    for (int i = 0; i < gNumber; ++i) {
        ++(*numref);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s number\n", argv[0]);
        return EXIT_FAILURE;
    }
    gNumber = atoi(argv[1]);

    int counter = 0;

    const int nthreads = 2;
    pthread_t threads[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        int s = pthread_create(&threads[i], NULL, thread_start, &counter);
        if (s != 0) {
            errno = s;
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nthreads; ++i) {
        int s = pthread_join(threads[i], NULL);
        if (s != 0) {
            errno = s;
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    printf("counter = %d\n", counter);
    
    return EXIT_SUCCESS;
}
